$fn = 64;

gear_diam = 90;
gear_thickness = 3;
gear_spur_depth = 2;

// Servo head
module servo_arm() {
  tip_diameter = 3.75;
  base_diameter = 7;
  short_arm_length = 15.7;
  long_arm_length = 18.1;
  bar_length = 16.45;
  arm_thickness = 1.75;
  base_thickness = 4;
  screw_height = 5;
  screw_diam = 4.2;

  // origin is center of arm attachment (screw center, without base)
  union() {
    linear_extrude(height=arm_thickness, center=true){
      union() {
        // arm and center
        hull() {
          circle(d=base_diameter);
          translate([long_arm_length - tip_diameter / 2, 0])
            circle(d=tip_diameter);
          translate([-(short_arm_length - tip_diameter / 2), 0])
            circle(d=tip_diameter);
        }

        // short bar across
        hull() {
          translate([0, bar_length / 2 - tip_diameter / 2])
            circle(d=tip_diameter);
          translate([0, -(bar_length / 2 - tip_diameter / 2)])
            circle(d=tip_diameter);
        }
      }
    }
    translate([0, 0, -base_thickness + arm_thickness / 2]) {
      cylinder(d=base_diameter, h=base_thickness);
      cylinder(d=screw_diam, h=screw_height);
    }
  }
}

difference() {
  translate([0, 0, 0.25])
    cube([37, 20, 2], center=true);
  servo_arm();
}
