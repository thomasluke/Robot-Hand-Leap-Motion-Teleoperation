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

arduino = serial.Serial('COM3', 9600)

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
        # print frame.hands.direction

        # print "Frame id: %d, timestamp: %d, hands: %d, fingers: %d, tools: %d, gestures: %d" % (
        #       frame.id, frame.timestamp, len(frame.hands), len(frame.fingers), len(frame.tools), len(frame.gestures()))

        # Get hands data from each frame
        # For loop repeats for each hand present in the frame
        for hand in frame.hands:
            finger_angles = []

            handType = "Left hand" if hand.is_left else "Right hand"

            # print "  %s, id %d, position: %s" % (
            #     handType, hand.id, hand.palm_position)

            # Get the hand's normal vector and direction
            normal = hand.palm_normal  # Normal vectors points outwards from palm
            direction = hand.direction  # Direction vector points from palm towards fingertips
            data_confidence = hand.confidence

            # print "DIRECTION:"
            # print direction

            # Calculate the hand's pitch, roll, and yaw angles
            # print "  pitch: %f degrees, roll: %f degrees, yaw: %f degrees" % (
            #     direction.pitch * Leap.RAD_TO_DEG,
            #     normal.roll * Leap.RAD_TO_DEG,
            #     direction.yaw * Leap.RAD_TO_DEG)

            # Get arm bone
            arm = hand.arm
            # print "  Arm direction: %s, wrist position: %s, elbow position: %s" % (
            #     arm.direction,  # Vector for direction arm is pointing towards hand
            #     arm.wrist_position,  # X, Y, Z coordinate for the locaton of centre of wrist
            #     arm.elbow_position)  # X, Y, Z coordinate for the locaton of centre of elbow

            resultant_vector_mag = [None] * 5
            index = 0
            # Get fingers
            for finger in hand.fingers:

                # iterator += 1
                # if iterator >=5:
                #     iterator = 0
                # # print "    %s finger, id: %d, length: %fmm, width: %fmm" % (
                # #     self.finger_names[finger.type],
                # #     finger.id,
                # #     finger.length,
                # #     finger.width)
                # if data_confidence >= 0.2:
                # finger_resultant_direction = finger.direction - direction
                # hand_vector, finger_vector = numpy.array([direction[0], direction[1], direction[2]]), numpy.array([finger.direction[0], finger.direction[1], finger.direction[2]])
                # resultant_vector = numpy.subtract(finger_vector, hand_vector)
                # resultant_vector_mag[iterator] = numpy.linalg.norm(resultant_vector)

                finger_vector = numpy.array([finger.direction[0],finger.direction[1],finger.direction[2]])
                hand_vector = numpy.array([hand.direction[0],hand.direction[1],hand.direction[2]])
                hand_normal_vector = numpy.array([hand.palm_normal[0],hand.palm_normal[1],hand.palm_normal[2]])
                
                if index == 0:
                    finger_angles.append(angle_between(finger_vector, hand_normal_vector))             
                else:
                    finger_angles.append(angle_between(finger_vector, hand_vector))

                index = index + 1

                # resultant_finger_vector = numpy.linalg.norm(finger_vector)

                # time.sleep(0.01)
                    # print (data_confidence)

                    # while True:
                    #     # time.sleep(2)
                    #     time.sleep(1)
                    #     arduino.write(100)
                    #     # bytesToRead = arduino.inWaiting()
                    #     # print arduino.read(bytesToRead).center
                    #     time.sleep(1)
                    #     arduino.write(0)
                    #     print "loop"

                # else:
                #     print "REALLIGN HAND: DATA CONFIDENCE TOO LOW"
                #     print (data_confidence)
                    
                # time.sleep(0.5)
                # data = resultant_vector_mag[1]
                # arduino.write(data)
                # angle = (numpy.dot(direction,finger.direction))/(numpy.dot(direction[0],finger.direction))
                # print (finger_resultant_direction),
                # Get bones
                for b in range(0, 4):
                    bone = finger.bone(b)
                    # print "      Bone: %s, start: %s, end: %s, direction: %s" % (
                    #     self.bone_names[bone.type],
                    #     bone.prev_joint,
                    #     bone.next_joint,
                    #     bone.direction)
            print("\n")
            
            # time.sleep(0.1)
            # if frame.hands.isLeft:
            #     print ("Left Hand")
            # elif frame.hands.isRight:
            #     print ("Right Hand")
            # else:
            #     print ("Hand Unknown")
            
            # for i in finger_angles:
            #     print finger_angles[int(i)]
            
            servo_angles = []
            index = 0
            while index <= 4:
                if index == 0:
                    servo_angles.append(int(abs((180*3)-(finger_angles[index]*(180/0.6)))))
                else:
                    servo_angles.append(int(finger_angles[index]*(180/3)))
                index = index + 1

            servo_angles.append('\n')
            # print servo_angles[4]
            # message = bytearray(servo_angles)
            # time.sleep(0.1)
            # arduino.write('ready')
            # test = servo_angles[2]
            arduino.write('\n')
            arduino.write(struct.pack('>5b', servo_angles[0]/3, servo_angles[1]/2, servo_angles[2]/2, servo_angles[3]/2, servo_angles[4]/2))
            print (str(servo_angles[0]) + "," + str(servo_angles[1]) + "," + str(servo_angles[2]) + "," + str(servo_angles[3]) + "," + str(servo_angles[4]))
            # time.sleep(0.1)
            # message = str(int(finger_angles[int(0)]*(180/3)))+"\n"+str(int(finger_angles[int(1)]*(180/3)))+"\n"+str(int(finger_angles[int(2)]*(180/3)))+"\n"+str(int(finger_angles[int(3)]*(180/3)))+"\n"+str(int(finger_angles[int(4)]*(180/3)))+"\n"
            # print message
            # arduino.write(struct.pack('>B', message))
            # arduino.write(json.dumps(servo_angles)) # https://arduino.stackexchange.com/questions/3753/how-to-send-numbers-to-arduino-uno-via-python-3-and-the-module-serial
            # arduino.write(struct.pack('>B', int(finger_angles[int(1)]*(180/3)))) # https://arduino.stackexchange.com/questions/3753/how-to-send-numbers-to-arduino-uno-via-python-3-and-the-module-serial
            # arduino.write(struct.pack('>B', int(finger_angles[int(2)]*(180/3)))) # https://arduino.stackexchange.com/questions/3753/how-to-send-numbers-to-arduino-uno-via-python-3-and-the-module-serial
            # arduino.write(struct.pack('>B', int(finger_angles[int(3)]*(180/3)))) # https://arduino.stackexchange.com/questions/3753/how-to-send-numbers-to-arduino-uno-via-python-3-and-the-module-serial
            # arduino.write(struct.pack('>B', int(finger_angles[int(4)]*(180/3)))) # https://arduino.stackexchange.com/questions/3753/how-to-send-numbers-to-arduino-uno-via-python-3-and-the-module-serial
            
            # print (angle_between(finger_vector, hand_vector)),
            # pointer_servo_angle = int(finger_angles[1]*(180/3))


            # print pointer_servo_angle
            # arduino.write(str(pointer_servo_angle))
            # values = bytearray(finger_angles)
            # ser.write(values)
            # arduino.write(struct.pack('>B', pointer_servo_angle)) # https://arduino.stackexchange.com/questions/3753/how-to-send-numbers-to-arduino-uno-via-python-3-and-the-module-serial
            # time.sleep(0.05)
            # reachedPos = str(arduino.readline())        # read serial port for arduino echo
            # print  reachedPos                               # print arduino echo to console
            # arduino.write(pack('>BBB,45,90,180'))




    #     # Get tools
    #     for tool in frame.tools:

    #         print "  Tool id: %d, position: %s, direction: %s" % (
    #             tool.id, tool.tip_position, tool.direction)

    #     # Get gestures
    #     for gesture in frame.gestures():
    #         if gesture.type == Leap.Gesture.TYPE_CIRCLE:
    #             circle = CircleGesture(gesture)

    #             # Determine clock direction using the angle between the pointable and the circle normal
    #             if circle.pointable.direction.angle_to(circle.normal) <= Leap.PI/2:
    #                 clockwiseness = "clockwise"
    #             else:
    #                 clockwiseness = "counterclockwise"

    #             # Calculate the angle swept since the last frame
    #             swept_angle = 0
    #             if circle.state != Leap.Gesture.STATE_START:
    #                 previous_update = CircleGesture(
    #                     controller.frame(1).gesture(circle.id))
    #                 swept_angle = (circle.progress -
    #                                previous_update.progress) * 2 * Leap.PI

    #             print "  Circle id: %d, %s, progress: %f, radius: %f, angle: %f degrees, %s" % (
    #                 gesture.id, self.state_names[gesture.state],
    #                 circle.progress, circle.radius, swept_angle * Leap.RAD_TO_DEG, clockwiseness)

    #         if gesture.type == Leap.Gesture.TYPE_SWIPE:
    #             swipe = SwipeGesture(gesture)
    #             print "  Swipe id: %d, state: %s, position: %s, direction: %s, speed: %f" % (
    #                 gesture.id, self.state_names[gesture.state],
    #                 swipe.position, swipe.direction, swipe.speed)

    #         if gesture.type == Leap.Gesture.TYPE_KEY_TAP:
    #             keytap = KeyTapGesture(gesture)
    #             print "  Key Tap id: %d, %s, position: %s, direction: %s" % (
    #                 gesture.id, self.state_names[gesture.state],
    #                 keytap.position, keytap.direction)

    #         if gesture.type == Leap.Gesture.TYPE_SCREEN_TAP:
    #             screentap = ScreenTapGesture(gesture)
    #             print "  Screen Tap id: %d, %s, position: %s, direction: %s" % (
    #                 gesture.id, self.state_names[gesture.state],
    #                 screentap.position, screentap.direction)

    #     if not (frame.hands.is_empty and frame.gestures().is_empty):
    #         print ""

    # def state_string(self, state):
    #     if state == Leap.Gesture.STATE_START:
    #         return "STATE_START"

    #     if state == Leap.Gesture.STATE_UPDATE:
    #         return "STATE_UPDATE"

    #     if state == Leap.Gesture.STATE_STOP:
    #         return "STATE_STOP"

    #     if state == Leap.Gesture.STATE_INVALID:
    #         return "STATE_INVALID"


def main():

    # arduino = serial.Serial('COM3', 9600)

    # while True:
    #     # time.sleep(2)
    #     time.sleep(1)
    #     arduino.write(100)
    #     # bytesToRead = arduino.inWaiting()
    #     # print arduino.read(bytesToRead).center
    #     time.sleep(1)
    #     arduino.write(0)
    #     print "loop"

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
