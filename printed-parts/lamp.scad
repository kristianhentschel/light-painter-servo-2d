include <common.scad>;

led_diam = 3;
led_base_diam = 3.75;
coin_diam = 18;
coin_thickness = 2;
clamp_diam = 3;
clamp_thickness = 1;

// origin shall be middle between two coins, at coin center.

module lamp_center() {

}

module lamp_clamp_rondell() {
  linear_extrude(height=clamp_thickness) {
    difference() {
      union() {
        square([coin_diam + 3 * clamp_diam, coin_diam / 2], center=true);
        circle(d=coin_diam);
      }

      translate([-(coin_diam / 2 + clamp_diam/2), 0])
        circle(d=outer_diam(clamp_diam));
    }
  }

  linear_extrude(height=2 * clamp_thickness + 3 * coin_thickness + led_diam) {
    translate([coin_diam / 2 + clamp_diam/2, 0])
      circle(d=clamp_diam);
  }
}

module lamp_clamp_hook() {
  clamp_height = 2 * coin_thickness;
  clamp_offset = 4 * coin_thickness;
  linear_extrude(height=coin_diam/2) {
    square([coin_diam + 3 * clamp_diam, clamp_thickness]);

    polygon([
      [clamp_thickness, 0],
      [clamp_thickness, clamp_offset],
      [clamp_thickness + clamp_diam/2, clamp_offset],
      [clamp_thickness, clamp_offset + clamp_height],
      [0, clamp_offset + clamp_height],
      [0, 0]
    ]);
  }
}

lamp_center();

lamp_clamp_rondell();
//lamp_clamp_hook();

//translate([coin_diam + 3 * clamp_diam + clamp_thickness + 0.1, 4 * coin_thickness - 0.1])
//rotate([0, 0, 180])
//lamp_clamp_hook();
