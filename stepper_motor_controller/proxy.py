from path_helpers import path
try:
    from base_node_rpc.proxy import ConfigMixinBase, StateMixinBase
    from .node import (Proxy as _Proxy, I2cProxy as _I2cProxy,
                       SerialProxy as _SerialProxy)
    from .config import Config
    from .state import State


    class ConfigMixin(ConfigMixinBase):
        @property
        def config_class(self):
            return Config


    class StateMixin(StateMixinBase):
        @property
        def state_class(self):
            return State


    class ProxyMixin(ConfigMixin, StateMixin):
        '''
        Mixin class to add convenience wrappers around methods of the generated
        `node.Proxy` class.

        For example, expose config and state getters/setters as attributes.
        '''
        host_package_name = str(path(__file__).parent.name.replace('_', '-'))

        @property
        def is_running(self):
            return self._is_running()

        def _is_running(self):
            return super(ProxyMixin, self).is_running()

        @property
        def steps_remaining(self):
            return self._steps_remaining()

        def _steps_remaining(self):
            return super(ProxyMixin, self).steps_remaining()

        @property
        def port(self):
            return self._stream.serial_device.port

        @property
        def steps_per_revolution(self):
            return self.config['steps_per_revolution']

        @steps_per_revolution.setter
        def steps_per_revolution(self, steps):
            return self.update_config(steps_per_revolution=steps)

        @property
        def microstep_setting(self):
            return self.config['microstep_setting']

        @microstep_setting.setter
        def microstep_setting(self, microstep_setting):
            return self.update_config(microstep_setting=microstep_setting)


    class Proxy(ProxyMixin, _Proxy):
        pass


    class I2cProxy(ProxyMixin, _I2cProxy):
        pass


    class SerialProxy(ProxyMixin, _SerialProxy):
        pass


except (ImportError, TypeError):
    Proxy = None
    I2cProxy = None
    SerialProxy = None
