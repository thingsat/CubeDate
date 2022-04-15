from posixpath import basename
import subprocess
import os
import sys

from factory import RIOTCtrlAppFactory
from riotctrl_shell.sys import Reboot, SUIT, SUITSequenceNoParser
from riotctrl_shell.vfs import VFS

PUBLISH_TARGET = "suit/publish-fw"

RIOTBASE = os.path.abspath(os.environ.get("RIOTBASE"))
DIRPATH = os.path.dirname(os.path.realpath(__file__))
APPLICATION_FS = f"{DIRPATH}/../../../apps/gcoap_fs"
APPLICATION_DUT = f"{DIRPATH}/../../cubedate"
BOARD = os.getenv("BOARD", "native")
UPDATING_TIMEOUT = 10
CSP = int(os.getenv("CSP", "1"))
SERVER_ADDR = "[2001:db8::2]" if CSP == 0 else "[::33]"
SUIT_COAP_SERVER = os.environ.get("SUIT_COAP_SERVER") or f"{SERVER_ADDR}/vfs/nvm0"


class VFSShell(Reboot, VFS):
    """Convenience class inheriting from the Reboot and VFS"""

    pass


class SUITShell(Reboot, SUIT):
    """Convenience class inheriting from the Reboot and SUIT"""

    _sequence_no = 0

    def parse_sequence_no(self):
        parser = SUITSequenceNoParser()
        self._sequence_no = parser.parse(self.suit_sequence_no())

    def sequence_no(self):
        return self._sequence_no


def publish(
    fs_shell: VFSShell,
    server_dir,
    server_url,
    app_ver,
    keys="default",
    latest_name=None,
):
    cmd = [
        "make",
        PUBLISH_TARGET,
        "APP_VER={}".format(app_ver),
        "RIOTBOOT_SKIP_COMPILE=1",
        "SUIT_KEY={}".format(keys),
    ]
    if latest_name is not None:
        cmd.append("SUIT_MANIFEST_SIGNED_LATEST={}".format(latest_name))
    assert not subprocess.call(cmd, cwd=APPLICATION_DUT)

    # remount file system updating files
    out = fs_shell.vfs_rmount(path="/nvm0")
    assert "Success" in out


def suit_current_seq_no(dut_shell: SUITShell) -> int:
    dut_shell.parse_sequence_no()
    return dut_shell.sequence_no()


def wait_for_update(child):
    return child.expect(
        [r"Fetching firmware \|[█ ]+\|\s+\d+\%", "Finalizing payload store"],
        timeout=UPDATING_TIMEOUT,
    )


def _get_manifest_path(seq_no: int) -> str:
    app = basename(APPLICATION_DUT)
    return f"coap://{SUIT_COAP_SERVER}/{app}/{BOARD}/riot_suit_{seq_no}.bin"


def _test_invalid_version(dut_shell: SUITShell, fs_shell: VFSShell, seq_no: int):
    publish(fs_shell, None, None, seq_no)
    res = dut_shell.suit_fetch(_get_manifest_path(seq_no))
    assert "suit_coap: trigger received" in res
    dut_shell.riotctrl.term.expect_exact("suit: verifying manifest signature")
    dut_shell.riotctrl.term.expect_exact("seq_nr <= running image")


def _test_invalid_key(dut_shell: SUITShell, fs_shell: VFSShell, seq_no: int):
    publish(fs_shell, None, None, seq_no + 1, keys="invalid_keys")
    res = dut_shell.suit_fetch(_get_manifest_path(seq_no + 1))
    assert "suit_coap: trigger received" in res
    dut_shell.riotctrl.term.expect_exact("suit: verifying manifest signature")
    dut_shell.riotctrl.term.expect_exact("Unable to validate signature")


def _test_successfull_update(dut_shell: SUITShell, fs_shell: VFSShell, seq_no: int):
    publish(fs_shell, None, None, seq_no + 1)
    res = dut_shell.suit_fetch(_get_manifest_path(seq_no + 1))
    assert "suit_coap: trigger received" in res
    dut_shell.riotctrl.term.expect_exact("suit: verifying manifest signature")
    dut_shell.riotctrl.term.expect_exact("SUIT policy check OK.")
    # wait for update to complete
    while wait_for_update(dut_shell.riotctrl.term) == 0:
        pass
    dut_shell.riotctrl.term.expect_exact("Verified installed payload")
    assert suit_current_seq_no(dut_shell) == seq_no + 1


def test(dut_shell: SUITShell, fs_shell: VFSShell):
    seq_no = suit_current_seq_no(dut_shell)
    _test_invalid_version(dut_shell, fs_shell, seq_no)
    _test_invalid_key(dut_shell, fs_shell, seq_no)
    _test_successfull_update(dut_shell, fs_shell, seq_no)


if __name__ == "__main__":
    with RIOTCtrlAppFactory() as factory:
        dut_env = {"BOARD": BOARD}
        fs_env = {"BOARD": BOARD}
        termargs = {}
        termargs["logfile"] = sys.stdout
        if BOARD == "native":
            if CSP == 1:
                fs_env.update({"VCAN_IFNAME": "vcan1"})
                dut_env.update({"VCAN_IFNAME": "vcan0"})
            else:
                dut_env.update({"PORT": "tap0"})
                fs_env.update({"PORT": "tap1"})
        fs_ctrl = factory.get_ctrl(
            env=fs_env,
            application_directory=APPLICATION_FS,
            termargs=termargs,
        )
        dut_ctrl = factory.get_ctrl(
            env=dut_env,
            application_directory=APPLICATION_DUT,
            termargs=termargs,
        )
        fs_shell = VFSShell(fs_ctrl)
        dut_shell = SUITShell(dut_ctrl)
        test(dut_shell=dut_shell, fs_shell=fs_shell)
