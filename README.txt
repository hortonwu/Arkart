1. add desktop shortcut of Qt5designer  
   1) using editor create an filename.desktop file in the path: usr/share/applications/
   2) copy the following code into the filename.desktop. To create and save a *.desktop file requires 
super user authorization run : $ sudo nano Qt5Designer.desktop 

**** Desktop Launcher***
[Desktop Entry]
Name=Qt5 Designer
Icon=/home/icons/qt5-designer.png
Exec=/usr/lib/x86_64-linux-gnu/qt5/bin/designer
Type=Application
Categories=Application
Terminal=false
StartupNotify=true
Actions=NewWindow

Name[en_US]=Qt5 Designer

[Desktop Action NewWindow]
Name=Open a New Window
Exec=/usr/lib/x86_64-linux-gnu/qt5/bin/designer

********************************************







2. convert *.ui ot python code

> pushd "C:\myFirstQtProject\"

Then type:
> pyuic5 -x firstgui.ui -o firstgui.py
or specify the complete path to pyuic5, for example:
> "C:\Program Files\Python34\Lib\site-packages\PyQt5\pyuic5.bat" -x firstgui.ui -o firstgui.py
