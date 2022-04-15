from contextlib import ContextDecorator
from riotctrl.ctrl import RIOTCtrlBoardFactory
from riotctrl_ctrl import native


class RIOTCtrlAppFactory(RIOTCtrlBoardFactory, ContextDecorator):
    def __init__(self):
        super().__init__(
            board_cls={
                "native": native.NativeRIOTCtrl,
            }
        )
        self.ctrl_list = list()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        for ctrl in self.ctrl_list:
            ctrl.stop_term()

    def get_ctrl(self, application_directory=".", env=None, termargs=None):
        # retrieve a RIOTCtrl Object
        ctrl = super().get_ctrl(env=env, application_directory=application_directory)
        # append ctrl to list
        self.ctrl_list.append(ctrl)
        # flash and start terminal
        ctrl.flash()
        ctrl.start_term(**termargs)
        # return ctrl with started terminal
        return ctrl
