#ifndef arch_drive
#define arch_drive

//Setup Archimedes Screw Drivetrain controller
void arch_drive_setup();

//Motion control links
void rightscrew_drive();
void leftscrew_drive();
void screw_drive();