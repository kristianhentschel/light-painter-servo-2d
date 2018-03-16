$fn = 64;

gear_diam = 90;
gear_thickness = 3;
gear_spur_depth = 2;
gear_rim_width = 3;

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

module servo_mount_mold_box() {
  difference() {
    translate([0, 0, 0.25])
      cube([37, 20, 2], center=true);
    servo_arm();
  }
}

module half_circle_rim(inner, outer) {
  difference() {
    circle(d=outer);
    circle(d=inner);
    translate([0, -outer/2])
      square([outer/2, outer]);
  }
}

module gear() {
  outer_diam = gear_diam + 2 * gear_spur_depth;
  inner_diam = gear_diam - 2 * gear_rim_width;

  difference() {
    union() {
      // half moon rim
      linear_extrude(height=gear_thickness) {
        half_circle_rim(inner_diam, outer_diam);
        translate([-gear_rim_width, -gear_diam/2])
          square([gear_rim_width, gear_diam]);

        // box that fits the servo mount
        resize([20, 40], center=true) circle(d=1);

        // arm 1
        rotate([0, 0, 60])
          square([gear_rim_width, gear_diam/2]);

        // arm 2
        rotate([0, 0, 120])
          square([gear_rim_width, gear_diam/2]);
      }
    }

    // spur cutout
    translate([0, 0, gear_thickness/3]) {
      linear_extrude(height=gear_thickness/3) {
        half_circle_rim(gear_diam, outer_diam+1);
      }
    }

    // servo arm mount cutout
    translate([0, 0, 1.75/2])
      rotate([0, 0, 90])
        servo_arm();

    // cable tie cutouts
    for(x = [-1, 1])
      for(y = [-1, 1])
        translate([x * 5, y * 6, 0])
          cylinder(d=3, h=gear_thickness);
  }

}

gear();
