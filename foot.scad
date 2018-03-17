include <common.scad>;

foot_length = 3 * stand_width;
foot_length = gear_diam * 2/3;

module foot() {
  linear_extrude(height = stand_thickness) {
    hull() {
      translate([-stand_width/2, stand_rim_width])
        square([stand_width, stand_thickness]);

      translate([-foot_length/2, 0])
        square([foot_length, stand_thickness]);
    }
  }
  linear_extrude(height = 2.25 * stand_thickness)
    translate([-stand_width/2, stand_rim_width])
      offset(r=-0.1)
        square([stand_width, stand_thickness]);
}

foot();
