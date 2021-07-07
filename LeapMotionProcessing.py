#!/usr/bin/env python

import os,sys
# src_dir = os.path.dirname(inspect.getfile(inspect.currentframe()))

# lib_dir = os.path.abspath(os.path.join(src_dir, ''))
# sys.path.insert(0, lib_dir)


# sys.path.append("C:\Users\Tom Harrison\Desktop\Subject Folders\Google Sync\Capstone\Code")
# sys.path.append("/path/to/lib/x86")
# sys.path.append("/path/to/lib") 
# dir_path = os.path.dirname(os.path.realpath(Leap.py))
# print (dir_path)

import Leap

import threading
import time
from Leap import CircleGesture, KeyTapGesture, ScreenTapGesture, SwipeGesture


class LeapMotionListener(Leap.Listener):
    finger_names = ['Thumb', 'Index', 'Middle', 'Ring', 'Pinky']
    bone_names = ['Metacarpal', 'Proximal', 'Intermediate', 'Distal']
    state_names = ['STATE_INVALID', 'STATE_START', 'STATE_UPDATE', 'STATE_END']

    def onInit(self, controller):  # Called when listener is intialised
        print("Listener Intialised")

    def onConnect(self, controller):  # Called when controller is intialised. Enables
        print("Motion Sensor Connected")

        # Enabling gestures for controller to detect. Motion sensor could not detect these kinds of gestures without the following lines.
        controller.enable_gesture(Leap.Gesture.TYPE_CIRCLE)
        controller.enable_gesture(Leap.Gesture.TYPE_KEY_TAP)
        controller.enable_gesture(Leap.Gesture.TYPE_SCREEN_TAP)
        controller.enable_gesture(Leap.Gesture.TYPE_SWIPE)

    def onDisconnect(self, controller):  # Called when controller is disconnected
        print("Motion Sensor Disconnected")

    def onExit(self, controller):  # Called when program in ended
        print("Exited")

    # LeapMotion sends about 290 frames of data to computer per second. This method gets called to access each frames data.
    def onFrame(self, controller):
        pass


def main():
    listener = LeapMotionListener()  # From this file
    controller = Leap.Controller()  # From Leap.py library

    # Adding my listener object to the Leap controller
    controller.add_listener(listener)

    print("Press Enter To Quit")
    try:  # Test for keyboard input
        sys.stdin.readline()
    except KeyboardInterrupt:  # If keyboard signal input then pass to finally
        pass
    finally:
        # Remove my listener object from the Leap controller after quitting
        controller.remove_listener(listener)


if __name__ == "__main__":
    main()
