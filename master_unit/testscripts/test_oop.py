import configparser

class TUConfig:
    # d_serialport = '/dev/ttyS4'
    # d_trafficservtimeout = 30 #seconds
    # d_trafficinterval = 10*60 #minutes
    # d_heartbeatinterval = 60*60 #minutes
    # d_slaveidlist = './idlist.txt'

    def __init__(self):
        self.serialport = '/dev/ttyS4'
        self.trafficservtimeout = 30 #seconds
        self.trafficinterval = 10*60 #minutes
        self.heartbeatinterval = 60*60 #minutes
        self.slaveidlist = './idlist.txt'

        conf = configparser.ConfigParser()
        try:
            conf.read('config.ini')

            if 'CUSTOM' in conf:
                customconf = conf['CUSTOM']

                self.serialport = customconf['SerialPort']
                self.trafficservtimeout = int(customconf['TrafficServerTimeout'])
                self.trafficinterval = int(customconf['TrafficDataInterval'])*60
                self.heartbeatinterval = int(customconf['HeartbeatInterval'])*60
                self.slaveidlist = customconf['SlaveIDList']

            elif 'DEFAULT' in conf:
                customconf = conf['DEFAULT']

                self.serialport = customconf['SerialPort']
                self.trafficservtimeout = int(customconf['TrafficServerTimeout'])
                self.trafficinterval = int(customconf['TrafficDataInterval'])*60
                self.heartbeatinterval = int(customconf['HeartbeatInterval'])*60
                self.slaveidlist = customconf['SlaveIDList']

        except FileNotFoundError:

            conf['DEFAULT'] = { \
                'SerialPort': self.serialport,
                'TrafficServerTimeout': str(self.trafficservtimeout),
                'TrafficDataInterval': str(self.trafficinterval // 60),
                'HeartbeatInterval': str(self.heartbeatinterval // 60),
                'SlaveIDList': self.slaveidlist }

            with open(configfilestring,'w') as configfile:
                conf.write(configfile)



    def importconfig(self, configfilestring):
        conf = configparser.ConfigParser()
        try:
            conf.read(configfilestring)

            if 'CUSTOM' in conf:
                customconf = conf['CUSTOM']
                self.serialport = customconf['SerialPort']
                self.trafficservtimeout = int(customconf['TrafficServerTimeout'])
                self.trafficinterval = int(customconf['TrafficDataInterval'])*60
                self.heartbeatinterval = int(customconf['HeartbeatInterval'])*60
                self.slaveidlist = customconf['SlaveIDList']
            else:
                raise FileStructureError('Settings need to be in [CUSTOM] section.')

        except FileNotFoundError:
            print("Error: " + configfilestring + "does not exist.")
            raise


config = TUConfig()
print(config.trafficservtimeout)
