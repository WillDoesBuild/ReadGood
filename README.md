# ReadGood
A handheld device that allows you to quickly read short stories.

## Hardware
I used these materials for the hardware:
1. Seeed Xiao RP2040
2. 122X250 LCD screen with I2C protocol communication (SDA and SCL)
3. 2 Relatively large pushbuttons
4. Perfboard, cut to size for the case
5. PLA plastic, black and white

## 3D models and printing
All models should be able to be printed without supports, except for the USB-C port on caseBase.stl. I printed it with an Bambu Labs A1 Mini. The CaseBase.stl should be printed with the "ReadGood" letters down so that they print cleanly. The CaseBase and CaseCap fit together with friction.

## Software
I have provided two versions of the software for this device. 
1. 'Full Program' means that you have to hard-code the text to be displayed as a string in the C++ code. In the provided program, I have about half of the first chapter of the Hunger Games book as an example.
2. 'Full Program With Text Upload' is the same as 'full program', but you need to upload a .txt file when prompted. Plug the SEEED into your computer and upload a .txt file into the USB port that appears.

## Limitations
1. Please keep in mind that this repository during a 3-day hackathon. The software is messy and the instructions may be incomplete. I am writing this at 3 AM lol.
2. Due to a possible hardware limitation with the RP2040 (I'm not entirely sure), the device can only hold a very limited number of characters (a little over 4230 characters). 'Full Program With Text Upload' can hold less, because it includes the software for txt uploads.
3. **'Full Program With Text Upload' loads the uploaded text into the RP2040's RAM. Unfortunately, this means that the text file is deleted from the device when it is unplugged from the computer. A future change may be to load supplied text into the flash memory, but I do not have time :( (see limitation 1)**
