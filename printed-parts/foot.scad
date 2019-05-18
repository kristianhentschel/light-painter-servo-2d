include <common.scad>;

foot_length = 3 * stand_width;
foot_length = gear_diam * 2/3;
foot_clip_height = 0.5;
foot_width = 2 * stand_width;

module foot() {
  difference() {
    union() {
      // foot
      linear_extrude(height = stand_thickness) {
        hull() {
          translate([-stand_width/2, stand_rim_width])
            square([stand_width, stand_thickness]);

          translate([-foot_length/2, -stand_thickness])
            square([foot_length, 2 * stand_thickness]);
        }
      }

      // hook
      linear_extrude(height = 2.5 * stand_thickness)
        translate([-stand_width/2, stand_rim_width])
          square([stand_width, stand_thickness]);

      // base
      linear_extrude(height = foot_width) {
        translate([-stand_width/2, -stand_thickness])
          square([stand_width, stand_thickness]);
      }

      // clip
      translate([-stand_width/2, 0, 2 * stand_thickness])
        rotate([90, 0, 90])
          linear_extrude(height = stand_width)
            polygon([
              [0, 0],
              [foot_clip_height, 0],
              [0, stand_thickness]
            ]);
    }

    // base cutout to get three point stand.
    translate([0, -stand_thickness, 0])
      rotate([-90, 0, 0])
        cylinder(d=foot_width * 2 - 2 * stand_thickness, h=stand_thickness/4);
  }
}

foot();
