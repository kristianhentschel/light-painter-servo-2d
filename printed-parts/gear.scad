// TODO: Screw mount is a bit too thick, cut another hole in it about halfway through
include <common.scad>;


// Servo head
module servo_arm() {
  tip_diameter = 3.75;
  base_diameter = 7;
  short_arm_length = 16.0;
  long_arm_length = 18.6;
  bar_length = 16.8;
  arm_thickness = 1.75;
  base_thickness = 4;
  screw_height = base_thickness + gear_thickness;
  screw_diam = 2.4;

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
      cylinder(d=outer_diam(screw_diam), h=screw_height);
    }
  }
}

module servo_mount_mold_box() {
  difference() {
    translate([0, 0, 0.25])
      cube([38, 20, 2], center=true);
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
      difference() {
        rotate_extrude(angle=180) {
          translate([inner_diam / 2, 0, 0])
            difference() {
              w = gear_rim_width + gear_spur_depth;
              h = gear_thickness;
              rt = h/6;
              square([w, h]);
              polygon([[w, rt], [w, h - rt], [w - gear_spur_depth, h/2 + rt/2], [w - gear_spur_depth, h/2 - rt/2]]);
            }
        }

        // cut off second half, as angle = 180 does not work in this version of openscad
        linear_extrude(height=gear_thickness)
          translate([gear_rim_width / 2, -outer_diam/2])
            square([outer_diam / 2, outer_diam]);
      }

      linear_extrude(height=gear_thickness) {
        // box that fits the servo mount
        resize([20, 40], center=true) circle(d=1);

        for (angle = [0, 60, 120, 180])
          rotate([0, 0, angle])
            translate([-gear_rim_width / 2, 0])
              square([gear_rim_width, gear_diam/2]);
      }
    }


    // servo arm mount cutout
    translate([0, 0, 1.75/2])
      rotate([0, 0, 90])
        servo_arm();

    // cable tie cutouts
    for(x = [-1, 1])
      for(y = [-1, 1])
        translate([x * 6, y * 6, 0])
          cylinder(d=outer_diam(3), h=gear_thickness);
  }
}

//rotate([180, 0 ,0])
  gear();
// servo_mount_mold_box();
