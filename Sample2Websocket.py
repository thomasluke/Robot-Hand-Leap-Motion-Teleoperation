################################################################################
# Copyright (C) 2012-2013 Leap Motion, Inc. All rights reserved.               #
# Leap Motion proprietary and confidential. Not for distribution.              #
# Use subject to the terms of the Leap Motion SDK Agreement available at       #
# https://developer.leapmotion.com/sdk_agreement, or another agreement         #
# between Leap Motion and you, your company or other organization.             #
################################################################################

import Leap
import sys
import thread
import time
import serial
import numpy
import struct
import json
from Leap import CircleGesture, KeyTapGesture, ScreenTapGesture, SwipeGesture
from serial import SerialException
from websocket import create_connection

# arduino = serial.Serial('COM3', 9600)
#  (http://127.0.0.1:6437)
# ws = create_connection("ws://localhost:8080/websocket")
ws = create_connection("ws://127.0.0.1:6437")
# print "Sending 'Hello, World'..."
# ws.send("Hello, World")
# print "Sent"
# print "Receiving..."
# result =  ws.recv()
# print "Received '%s'" % result
# ws.close()

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

    def on_frame(self, controller):
        # Get the most recent frame and report some basic information
        frame = controller.frame()  # Frame sent from Leap Motion Controller to computer

        result = ws.recv()
        resultj = json.loads(result)
        # print resultj
        # resultj["hands"]

        # Get hands data from each frame
        # For loop repeats for each hand present in the frame
        for hands in resultj:

            # handType = "Left hand" if hands.is_left else "Right hand"

            # Get the hand's normal vector and direction
            normal = ["palmNormal"]  # Normal vectors points outwards from palm
            direction = ["direction"]  # Direction vector points from palm towards fingertips
            # data_confidence = hands.confidence           
            # arm = hands.arm
            print normal
            resultant_vector_mag = [None] * 5
            
            # Reset variables
            finger_flexion = []
            servo_angles = []
            index = 0
            
            # Get fingers
            for fingers in hands.fingers:

                # Converting finger and hand vectors in numpy library arrays
                finger_vector = numpy.array([finger.direction[0],finger.direction[1],finger.direction[2]])
                hand_vector = numpy.array([hands.direction[0],hands.direction[1],hands.direction[2]])
                hand_normal_vector = numpy.array([hands.palm_normal[0],hands.palm_normal[1],hands.palm_normal[2]])
                
                # If tracked finger is thumb, then measure its flexion angle with respect to the hand/palm normal vector
                if index == 0:
                    # Calculating angle between finger and hand vectors ensures that hand orientation does not matter.
                    finger_flexion.append(angle_between(finger_vector, hand_normal_vector)) 
                    # Conversion factor finger flexion values to servo motor rotation angle.  
                    servo_angles.append(int(abs((180*3)-(finger_flexion[index]*(180/0.6)))))          
                
                # If the tracked finger is not a thumb then measure its flexion angle with respect to the hand/palm vector
                else:
                    # Calculating angle between finger and hand vectors ensures that hand orientation does not matter.
                    finger_flexion.append(angle_between(finger_vector, hand_vector))
                    # Conversion factor finger flexion values to servo motor rotation angle.  
                    servo_angles.append(int(finger_flexion[index]*(180/3)))
                
                # Iterate index value used for array indexing
                index = index + 1

            # # Adds a line space inbetween terminal messages
            # print("\n")
            
            # Serial write newline character which to inidcate where to start reading bytes in the Arduino code
            # arduino.write('\n')
            
            # Serial write servo rotation angles in byte (binary) form to the Arduino code. Divide values sent to keep in the required byte range of -128<=value<=128. Values multiplied back in Arduino code. 
            # arduino.write(struct.pack('>5b', servo_angles[0]/3, servo_angles[1]/2, servo_angles[2]/2, servo_angles[3]/2, servo_angles[4]/2))
            
            # Print terminal message showing servo motor rotation values being sent to Arduino via serial communication
            # print ("Thumb servo angle: " + str(servo_angles[0]) + ", " + "Pointer servo angle: " + str(servo_angles[1]) + ", " + "Index servo angle: " + str(servo_angles[2]) + ", " + "Ring servo angle: " + str(servo_angles[3]) + ", " + "Pinky servo angle: " + str(servo_angles[4]))
            # time.sleep(0.1)

def main():

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


if __name__ == "__main__":
    main()
