include <common.scad>;
use <stand.scad>;

idler_diam = stand_rim_width;

module idler() {
  cylinder(d=idler_diam, h=gear_thickness);
  translate([0, 0, gear_thickness - 1])
    cylinder(d=2 * idler_diam, h=1);
}

gear_to_stand = gear_hood_height + arm_spacing + stand_thickness + (body_height - mount_offset);

module guide() {
  translate([0, gear_diam/2 + gear_spur_depth + idler_diam, 0]) {
    translate([0, 0, gear_to_stand]) {
      idler();
      rotate([-90, 0, 0])
        idler();
    }
  }


}

beam_length = stand_height + 2 * body_height;

module beam() {
  translate([0, 0, gear_diam/2-stand_rim_width])
    linear_extrude(height=stand_thickness) {
      // mounting points
      translate([-stand_width / 2, -stand_thickness * 2])
        square([stand_width, stand_thickness * 2]);
      translate([-stand_width / 2, beam_length + 2 * stand_rim_width])
        square([stand_width, stand_thickness * 2]);
      // beam
      translate([0, stand_rim_width])
        difference() {
          offset(r=stand_rim_width)
            translate([-stand_width/2, 0, 0])
              square([stand_width, beam_length]);
          triangle_cutouts(10, beam_length);
        }
    }
}


union() {
  guide();
  beam();
}

rotate([90, 0, 0]) {
  stand();
  translate([0, 0, -beam_length-stand_thickness-2*stand_rim_width])
    stand();
}
