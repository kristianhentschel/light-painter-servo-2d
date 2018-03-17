include <common.scad>;
use <stand.scad>;

idler_diam = stand_rim_width;

module idler() {
  cylinder(d=idler_diam, h=gear_thickness);
  translate([0, 0, gear_thickness])
    cylinder(d=2*idler_diam, h=1);
}

gear_to_stand = gear_hood_height + arm_spacing + stand_thickness + (body_height - mount_offset);

module guide() {
  translate([0, -gear_to_stand, 2 * stand_rim_width]) {
    translate([-stand_thickness/2, 0, 0])
      cube([stand_thickness, gear_to_stand, 3]);
    translate([stand_thickness, 0, 0]) {
      idler();
      rotate([90, 0, 0])
        idler();
    }
    translate([-stand_thickness / 2, 0, -idler_diam/2])
      cube([idler_diam + stand_thickness, idler_diam, 2 * stand_thickness]);
  }
}

beam_length = stand_height + 2 * body_height;
beam_width = 6;
beam_support_height = 2 * stand_rim_width;

module half_beam() {
  translate([0, 0, gear_diam/2-stand_rim_width]) {
    // mounting point
    translate([-stand_width / 2, -stand_thickness * 2, 0])
      cube([stand_width, stand_thickness * 2, stand_thickness]);

    // base
    translate([-beam_width/2, 0, 0])
      cube([beam_width, beam_length/2, stand_thickness]);

    // vertical support and hook
    translate([-beam_width / 2 + stand_thickness / 2, 0, stand_thickness]){
      rotate([90, 0, 90])
        linear_extrude(height=stand_thickness)
          polygon([
            [0, 0],
            [0, stand_rim_width],
            [-stand_thickness, stand_rim_width],
            [-stand_thickness, stand_rim_width - 0.5],
            [-2 * stand_thickness, stand_rim_width],
            [-2 * stand_thickness, beam_support_height],
            [0, beam_support_height],
            [beam_length/2, 0]
          ]);
    }

    translate([stand_thickness/2, body_height - idler_diam, idler_diam])
      rotate([90, 0, 90])
        idler();

    translate([0, -stand_thickness])
      guide();
  }
}

module beam() {
  half_beam();
  translate([0, beam_length, 0])
    mirror([0, 1, 0])
      half_beam();
}

// beam();
//
// rotate([90, 0, 0]) {
//   stand();
//   translate([0, 0, -beam_length-stand_thickness])
//     stand();
// }

difference() {
  half_beam();
  translate([-10, body_height, 0])
  cube([20, 60, 100]);
}
