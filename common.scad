$fn = 100;

function outer_diam(d, fn=$fn) = 1/cos(180/fn) * d;

// gear
gear_diam = 90;
gear_thickness = 4;
gear_spur_depth = 2;
gear_rim_width = 3;

// stand (vertical beam with servo mount)
gear_diameter = gear_diam;
body_length = 23.2;
body_width = 12.5;
body_height = 22.6;
mount_height = 2.8;
mount_offset = 21 - mount_height;
mount_length = 32.5;
gear_hood_diam = 12;
gear_hood_height = 5.7;
arm_spacing = 2.5;
mount_hole_diam = 1.75;
mount_hole_offset = (27.8 - body_length)/2;
stand_rim_width = 3;
stand_thickness = 3;
stand_height = 120;
stand_width = 10;
