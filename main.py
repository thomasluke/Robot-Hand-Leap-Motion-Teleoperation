################################################################################
# Copyright (C) 2012-2013 Leap Motion, Inc. All rights reserved.               #
# Leap Motion proprietary and confidential. Not for distribution.              #
# Use subject to the terms of the Leap Motion SDK Agreement available at       #
# https://developer.leapmotion.com/sdk_agreement, or another agreement         #
# between Leap Motion and you, your company or other organization.             #
################################################################################

import Leap
import sys
import threading
import time
import serial
import numpy
import struct
import json
import keyboard
import time
import csv
from Leap import CircleGesture, KeyTapGesture, ScreenTapGesture, SwipeGesture
from serial import SerialException

arduino = serial.Serial('COM3', 9600, timeout=0.1)
# self.servo_angles = []

def unit_vector(vector):
    """ Returns the unit vector of the vector.  """
    return vector / numpy.linalg.norm(vector)

def angle_between(v1, v2):
    """ Returns the angle in radians between vectors 'v1' and 'v2'::

            >>> angle_between((1, 0, 0), (0, 1, 0))
            1.5707963267948966
            >>> angle_between((1, 0, 0), (1, 0, 0))
            0.0
            >>> angle_between((1, 0, 0), (-1, 0, 0))
            3.141592653589793
    """
    v1_u = unit_vector(v1)
    v2_u = unit_vector(v2)
    return numpy.arccos(numpy.clip(numpy.dot(v1_u, v2_u), -1.0, 1.0))

class SampleListener(Leap.Listener):
    
    finger_names = ['Thumb', 'Index', 'Middle', 'Ring', 'Pinky']
    bone_names = ['Metacarpal', 'Proximal', 'Intermediate', 'Distal']
    state_names = ['STATE_INVALID', 'STATE_START', 'STATE_UPDATE', 'STATE_END'] 

    def on_init(self, controller):
        print "Initialized"
        self.start_leap = time.time()
        self.end_leap = time.time()
        self.latency_leap = 0.0

        self.start_arduino = time.time()
        self.end_arduino = time.time()
        self.latency_arduino = 0.0

        self.latency_total_start = time.time()
        self.latency_total_end = time.time()
        self.latency_total = 0.0

        self.iterator = 0.1
        self.rows = []
        self.number = 0
        self.lock = False
        self.lock2 = False

        self.servo_angles = [0,0,0,0,0]
        self.data_confidence = 0
        self.roll = 0

    def on_connect(self, controller):
        print "Connected"

        # Enable gestures
        controller.enable_gesture(Leap.Gesture.TYPE_CIRCLE)
        controller.enable_gesture(Leap.Gesture.TYPE_KEY_TAP)
        controller.enable_gesture(Leap.Gesture.TYPE_SCREEN_TAP)
        controller.enable_gesture(Leap.Gesture.TYPE_SWIPE)

    def on_disconnect(self, controller):
        # Note: not dispatched when running in a debugger.
        print "Disconnected"

    def on_exit(self, controller):
        print "Exited"

    def run(self):
        # threading.Thread(target = main, args=(self,)).start()
        # threading.Thread(target = self.measure_latency).start()
        threading.Thread(target = self.measure_gestures).start()
    
    def measure_latency(self):

            while True:

                time.sleep(0.02)

                if arduino.in_waiting>=1:
                    if arduino.read() == "\n":
                        self.latency_arduino = int(arduino.read_until(expected = "\n"))
                        arduino.reset_input_buffer()

                    self.number+=1

                    self.latency_total_end = time.time()
                    self.latency_total = (self.latency_total_end - self.latency_total_start) * 1000
                    self.latency_total_start = time.time()

                    latency_serial = self.latency_total - self.latency_arduino - self.latency_leap
                    # print("Latency Leap (ms): " + str(self.latency_leap) + " Latency Arduino (ms): " + str(self.latency_arduino) + " Latency Serial (ms): " + str(latency_serial) + " Latency total (ms): " + str(self.latency_total))

                    self.rows.append([str(self.number),str(self.latency_leap),str(self.latency_arduino),str(latency_serial),str(self.latency_total)])
                
                    if keyboard.is_pressed("s"):
                            
                        fields = ["Number", "Latency Leap", "Latency Arduino", "Latency Serial", "Latency Total"]

                        # name of csv file 
                        filename = "Latency Data.csv"
                
                        # writing to csv file 
                        with open(filename, 'wb') as csvfile: 
                            # creating a csv writer object 
                            csvwriter = csv.writer(csvfile) 
                                
                            # writing the fields 
                            csvwriter.writerow(fields) 
                                
                            # writing the data rows 
                            csvwriter.writerows(self.rows)
                        
                        print "LATENCY DATA SAVED TO CSV FILE" 

    def measure_gestures(self):
        
        if arduino.in_waiting>=1:
            if arduino.read() == "\n":
                index = 0
                serial_finger_angles = [0,0,0,0,0]
                while index <5: 
                    serial_finger_angles[index] = int(arduino.read_until(expected = "\n"))
                    arduino.reset_input_buffer()
                    if self.lock2 == False:
                        self.latency_total_start = time.time()
                        self.lock2 == True
                    index = index + 1
                print serial_finger_angles
                # if (serial_finger_angles[0]>=180 and serial_finger_angles[1]>=180 and serial_finger_angles[2]<=35 and serial_finger_angles[3]<=35 and serial_finger_angles[4]>=180):
                #     self.latency_total_end = time.time()
                #     self.latency_total = (self.latency_total_end - self.latency_total_start) * 1000

                #     print "Pose 1 Achieved in " + str(self.latency_total) + " seconds. Change to pose 2"

                    # self.latency_total_start = time.time()

        
    
    def on_frame(self, controller):
        time.sleep(0.003)
        # Get the most recent frame and report some basic information
        frame = controller.frame()  # Frame sent from Leap Motion Controller to computer

        if self.lock == False:
                
            self.run()
            self.lock = True

        self.iterator = self.iterator + 1

        # Get hands data from each frame
        # For loop repeats for each hand present in the frame
        for hand in frame.hands:

            handType = "Left hand" if hand.is_left else "Right hand"

            # Get the hand's normal vector and direction
            normal = hand.palm_normal  # Normal vectors points outwards from palm
            direction = hand.direction  # Direction vector points from palm towards fingertips
            self.data_confidence = hand.confidence           
            arm = hand.arm
            self.roll = int((normal.roll * Leap.RAD_TO_DEG)+90)

            resultant_vector_mag = [None] * 5
            
            # Reset variables
            finger_flexion = []
            self.servo_angles = []
            index = 0
            
            # Get fingers
            for finger in hand.fingers:

                # Converting finger and hand vectors in numpy library arrays
                finger_vector = numpy.array([finger.direction[0],finger.direction[1],finger.direction[2]])
                hand_vector = numpy.array([hand.direction[0],hand.direction[1],hand.direction[2]])
                hand_normal_vector = numpy.array([hand.palm_normal[0],hand.palm_normal[1],hand.palm_normal[2]])
                
                # If tracked finger is thumb, then measure its flexion angle with respect to the hand/palm normal vector
                if index == 0:
                    # Calculating angle between finger and hand vectors ensures that hand orientation does not matter.
                    finger_flexion.append(angle_between(finger_vector, hand_normal_vector)) 
                    # Conversion factor finger flexion values to servo motor rotation angle.  
                    self.servo_angles.append(int(abs((180*3)-(finger_flexion[index]*(180/0.6))))) 
                
                # If the tracked finger is not a thumb then measure its flexion angle with respect to the hand/palm vector
                else:
                    # Calculating angle between finger and hand vectors ensures that hand orientation does not matter.
                    finger_flexion.append(angle_between(finger_vector, hand_vector))
                    # Conversion factor finger flexion values to servo motor rotation angle. 
                    
                    self.servo_angles.append(int(finger_flexion[index]*(180/2.5)))
                
                # Iterate index value used for array indexing
                index = index + 1

            self.end_leap = time.time()
            self.latency_leap = (self.end_leap - self.start_leap)*1000
            self.start_leap = time.time()
          
            # arduino.reset_output_buffer()

            # # Serial write newline character which to inidcate where to start reading bytes in the Arduino code
            # arduino.write('\n')
                    
            # Serial write servo rotation angles in byte (binary) form to the Arduino code. Divide values sent to keep in the required byte range of -128<=value<=128. Values multiplied back in Arduino code. 
            arduino.write(struct.pack('>8b',int(0),self.servo_angles[0]/4, self.servo_angles[1]/4, self.servo_angles[2]/4, self.servo_angles[3]/4, self.servo_angles[4]/4, self.roll/4, int(self.data_confidence*100)))
            # arduino.write(struct.pack('>7b',self.servo_angles[0]/4, self.servo_angles[1]/4, self.servo_angles[2]/4, self.servo_angles[3]/4, self.servo_angles[4]/4, self.roll/4, int(self.data_confidence*100)))
                       # arduino.reset_output_buffer()
            # print arduino.out_waiting

            self.iterator = 0

            # print "Finger Angles: " + str(self.servo_angles) + " Leap Data Confidence: " + str(int(self.data_confidence*100))
            
def measure_latency(control_mode, lock = False):

    if lock == False:
        
        number = 0
        rows = []
        arduino_latency = 0

        total_latency_start = time.time()
        local_latency_start = time.time()

        lock = True

    # while True:
    local_latency_end = time.time()
    local_latency = (local_latency_end - local_latency_start) * 1000
    local_latency_start = time.time()

    if arduino.in_waiting>=1:
        if arduino.read() == "\n":
            arduino_latency = int(arduino.read_until(expected = "\n"))
            arduino.reset_input_buffer()
    
        number+=1

        total_latency_end = time.time()
        total_latency = (total_latency_end - total_latency_start) * 1000
        total_latency_start = time.time()

        serial_latency = total_latency - arduino_latency - local_latency
        print(" Latency Arduino (ms): " + str(arduino_latency) +  " Latency Local (ms): " + str(local_latency) + " Latency Total (ms): " + str(total_latency) + " Latency Serial (ms): " + str(serial_latency))

        rows.append([str(number),str(arduino_latency),str(local_latency),str(total_latency),str(serial_latency)])

    if keyboard.is_pressed("s"):
            
        fields = ["Number", "Latency Arduino", "Latency Local"]

        # name of csv file 
        filename = control_mode + " Latency Data.csv"

        # writing to csv file 
        with open(filename, 'wb') as csvfile: 
            # creating a csv writer object 
            csvwriter = csv.writer(csvfile) 
                
            # writing the fields 
            csvwriter.writerow(fields) 
                
            # writing the data rows 
            csvwriter.writerows(rows)
        
        print "LATENCY DATA SAVED TO CSV FILE" 

def main():
    
    print "Select Control Mode: Please type 1, 2, 3, 4, 5 or 6"
    print "Mode 1: Automatic Control Selection" # Switches automatically between Leap Motion and glove control based on whether the Leap Motion is receiving frame data
    print "Mode 2: Leap Motion Control" # Leap Motion hand tracking control
    print "Mode 3: Glove Control" # Glove hand tracking control
    print "Mode 4: Keyboard Control" # Shift, q, w, e, r keyboard press control
    print "Mode 5: Combined Control " # Combined leap motoon and glove hand tracking control for potentially imporved accuracy
    print "Mode 6: Combined Control Interpolated Leap Data" # Combined leap motoon and glove hand tracking control for potentially imporved accuracy


    while 1:

        mode = raw_input()

        if mode == "1" or mode == "2" or mode == "3" or mode == "4" or mode == "5" or mode == "6": 
            
            print "Control mode " + mode + " selected"
            
            # Serial write newline character which to inidcate where to start reading bytes in the Arduino code
            arduino.write('\n')
                
            # Serial write servo rotation angles in byte (binary) form to the Arduino code.
            arduino.write(struct.pack('>1b', int(mode)))

            break
            
        else:

            print "Invalid mode selected. Please type '1', '2', '3', '4', '5' or '6'"

    if mode == "1" or mode == "2" or mode == "4" or mode == "5" or mode == "6":

        # Create a sample listener and controller
        listener = SampleListener()
        controller = Leap.Controller()
        
        # Have the sample listener receive events from the controller
        controller.add_listener(listener)

        # Keep this process running until Enter is pressed
        print "Press Enter to quit..."
        try:
            sys.stdin.readline()
        except KeyboardInterrupt:
            pass
        finally:
            # Remove the sample listener when done
            controller.remove_listener(listener)
    elif mode == "3":
        print "test"
        # while True:
        #     measure_latency("Glove")
        # while True:
        #     # while True:
        #     #     measure_latency("Glove")
        #     # if arduino.in_waiting>=1:
        #     if arduino.read() == "\n":
        #         index = 0
        #         serial_finger_angles = [0,0,0,0,0]
        #         while index <5: 
        #             serial_finger_angles[index] = (arduino.read_until(expected = " "))
        #             # if self.lock2 == False:
        #             #     self.latency_total_start = time.time()
        #             #     self.lock2 == True
        #             index = index + 1
        #         print serial_finger_angles
        #     arduino.reset_input_buffer()

            
    elif mode == "4":

        while True:

            self.servo_angles = []

            finger_keys = ["shift", "q", "w", "e", "r"]

            for key in finger_keys:

                if keyboard.is_pressed(key):
                    self.servo_angles.append(180)
                else:
                    self.servo_angles.append(0)
                    # self.servo_angles[idx] = 180       

            # Serial write newline character which to inidcate where to start reading bytes in the Arduino code
            arduino.write('\n')
                
            # Serial write servo rotation angles in byte (binary) form to the Arduino code. Divide values sent to keep in the required byte range of -128<=value<=128. Values multiplied back in Arduino code. 
            arduino.write(struct.pack('>5b', self.servo_angles[0]/2, self.servo_angles[1]/2., self.servo_angles[2]/2, self.servo_angles[3]/2, self.servo_angles[4]/2))

            # Print terminal message showing servo motor rotation values being sent to Arduino via serial communication
            print "Finger Angles: " + self.servo_angles
            # time.sleep(0.1)
            
            measure_latency("Keyboard")
    
if __name__ == "__main__":
    main()
