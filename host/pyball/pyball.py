#!/usr/bin/python2

import Queue
import threading
import socket
import time
import os
import sys
import pygame
import math

########################################################################

class ProbeReader(threading.Thread):

    def __init__(self):
        threading.Thread.__init__(self)
        self.__q__ = Queue.Queue()
    
    def run(self):
        self.__setup__()
        while True:
            self.__q__.put(self.__infile__.readline().strip())
            
    def __setup__(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('192.168.4.1', 80))
        self.__infile__ = s.makefile()

    def get(self):
        try:
            return self.__q__.get_nowait()
        except Queue.Empty:
            return None

########################################################################

class ProbeData:

    def __init__(self, fields):
        self.__translate__(fields)

    def type(self):
        pass

    def __translate__(self, fields):
        self.seq = int(fields[0])

class ProbeAirData(ProbeData):

    def __init__(self, fields):
        ProbeData.__init__(self, fields)

    def type(self):
        return 'air'

    def __translate__(self, fields):
        ProbeData.__translate__(self, fields)
        pass # TODO
    
class ProbeAirReducedData(ProbeData):

    def __init__(self, fields):
        ProbeData.__init__(self, fields)

    def type(self):
        return 'air_reduced'

    def __translate__(self, fields):
        ProbeData.__translate__(self, fields)
        self.alpha = float(fields[1])
        self.beta = float(fields[2])
        self.q = float(fields[3])
        self.p = float(fields[4])
        self.t = float(fields[5])

class ProbeBatteryData(ProbeData):

    def __init__(self, fields):
        ProbeData.__init__(self, fields)

    def type(self):
        return 'battery'

    def __translate__(self, fields):
        # ProbeData.__translate__(self, fields)
        pass # TODO

class ProbeUnknownData(ProbeData):

    def __init__(self, type, fields):
        ProbeData.__init__(fields)
        self.__type__ = type

    def type(self):
        return self.__type__

    def __translate__(self, fields):
        ProbeData.__translate__(self, fields)
        pass # TODO
    
def get_probe_data(line):
    fields = line.split(',')
    if fields[0] == '$A':
        return ProbeAirData(fields[1:])
    if fields[0] == '$AR':
        return ProbeAirReducedData(fields[1:])
    if fields[0] == '$B':    
        return ProbeBatteryData(fields[1:])
    return ProbeUnknownData(fields[0], fields[1:])
    
########################################################################
        
class ProbeDataSource:

    def __init__(self):
        self.__probe_reader__ = ProbeReader()
        self.__probe_reader__.start()
        self.__of__ = open('data.csv', 'a')
    
    def get(self):
        line = self.__probe_reader__.get()
        if line == None: return None
        self.__of__.write("%s,%s\n" % (time.time(), line))
        self.__of__.flush()
        return get_probe_data(line)

########################################################################

class Application:

    def __init__(self):
        self.__init_pygame__()
        
    def run(self):
        pass

    def __init_regular__(self):
        pygame.display.init()
        self.__screen__ = pygame.display.set_mode([800, 480])

    def __init_framebuffer__(self):
        drivers = ['fbcon', 'directfb', 'svgalib']
        found = False
        for driver in drivers:
            # Make sure that SDL_VIDEODRIVER is set
            if not os.getenv('SDL_VIDEODRIVER'):
                os.putenv('SDL_VIDEODRIVER', driver)
                try:
                    pygame.display.init()
                    pass
                except pygame.error:
                    print('Driver: {0} failed.'.format(driver))
                    continue
            found = True
            break
        if not found:
            raise Exception('No suitable video driver found!')
        screen_size = (
            pygame.display.Info().current_w,
            pygame.display.Info().current_h,
        )
        self.__screen__ = pygame.display.set_mode(screen_size, pygame.FULLSCREEN)
        
    def __init_pygame__(self):
        self.__init_regular__()
        self.__screen__.fill((0, 0, 0))                
        pygame.font.init()
        pygame.display.update()
        self.__clock__ = pygame.time.Clock()

    def screen(self):
        return self.__screen__

    def clock(self):
        return self.__clock__
    
########################################################################

class DataView(Application):
    
    def __init__(self):
        Application.__init__(self)
        self.probe_data = ProbeDataSource()

    def run(self):
        done = False
        while not done:
            try:
                done = self.__one_cycle__()
            except Exception, e:
                print(e)
                done = True
        pygame.quit()
        os._exit(1) # kill lingering threads

    def __one_cycle__(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return True
            if event.type == pygame.KEYDOWN:
                return True
        while True:
            v = self.probe_data.get()
            if v == None:
                break
            self.handle_data(v)
        self.clock().tick(60)
        pygame.display.flip()
        return False
    
    def handle_data(self, v):
        pass
        
########################################################################

class SimpleDataView(DataView):

    __box_unit__ = 50
    __font_size__ = 40
    __padding__ = 4

    __dry_air_gas_constant__ = 287.058
    __standard_air_density__ = 1.225;
    
    def __init__(self):
        DataView.__init__(self)
        self.__last_seq__ = None
        self.__font__ = pygame.font.Font('freesansbold.ttf', self.__font_size__)
    
    def handle_data(self, v):
        if v.type() != 'air_reduced':
            return

        if self.__last_seq__ == None:
            self.__last_seq__ = v.seq
        else:
            if v.seq != (self.__last_seq__ + 1):
                raise Exception('Out of sequence error')
            self.__last_seq__ = v.seq

        self.screen().fill((0, 0, 0))
        self.show_data([
            ['alpha', 'deg', "%03.1f", v.alpha ],
            ['beta', 'deg', "%03.1f", v.beta ],
            ['p', 'Pa', "%06.0f", v.p ],
            ['q', 'Pa', "%03.0f", v.q ],
            ['T', 'deg C', "%03.1f", v.t ],
            ['p alt', 'ft', "%03.0f", self.meters_to_feet(self.pressure_altitude(v.t, v.p)) ],
            ['ias', 'kias', "%03.0f", self.meters_per_second_to_knots(self.indicated_airspeed(v.q)) ],
            ['tas', 'ktas', "%03.0f", self.meters_per_second_to_knots(self.true_airspeed(v.q, v.p, v.t)) ],
        ])

    def show_data(self, data):
        for i in range(0, len(data)):
            d = data[i]
            self.text(0, i, d[0] + ':')
            self.text(3, i, d[2] % d[3])
            self.text(7, i, d[1])
        
    def text(self, x, y, str):
        size = self.__font__.size(str)
        fg = 0xff, 0xff, 0xff
        bg = 0, 0, 0
        ren = self.__font__.render(str, 0, fg, bg)
        self.screen().blit(
            ren, (
                x * self.__box_unit__ + self.__padding__,
                y * self.__box_unit__ + self.__padding__,
            ))

    def float_to_string(self, x):
        return "%0.3f" % x
        
    def pressure_altitude(self, t, p, qnh=101325.0):
        ratio = (qnh / p);
        t_kelvin = t + 273.15;
        power = 1 / 5.257;
        return (pow(ratio, power) - 1.0) * t_kelvin / 0.0065

    def true_airspeed(self, q, p, t):
        return math.sqrt(2.0 * q / self.dry_air_density(p, t))

    def indicated_airspeed(self, q):
        return math.sqrt(2.0 * q / self.__standard_air_density__)
    
    def dry_air_density(self, p, t):
        return p / (self.__dry_air_gas_constant__ * self.celsius_to_kelvin(t))

    def celsius_to_kelvin(self, x):
        return x + 273.15

    def meters_to_feet(self, x):
        return x * 3.28084

    def meters_per_second_to_knots(self, x):
        return x * 1.94384
    
########################################################################

SimpleDataView().run()
