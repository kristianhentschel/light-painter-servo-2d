use <gear.scad>;

$fn = 100;

gear_diameter = 90;
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

module servo () {
  // origin is mounting point, center of gear attachment
  offset = [
    -(body_length - gear_hood_diam/2),
    -(body_width / 2),
    -(body_height + gear_hood_height + arm_spacing)
  ];

  translate(offset) {
    union() {
      cube([body_length, body_width, body_height]);
      translate([-(mount_length - body_length)/2, 0, mount_offset])
        cube([mount_length, body_width, mount_height]);
      translate([body_length - gear_hood_diam/2, body_width/2, body_height])
        cylinder(d=gear_hood_diam, h=gear_hood_height);
    }
  }
}

module example_gear_and_servo() {
  rotate([90, 0, 90]) {
    rotate([0, 0, 90])
      servo();
    gear();
  }
}

module servo_mount_cutout() {
  translate([-body_width/2, -(body_length - gear_hood_diam /2)]) {
    square([body_width, body_length]);
    translate([body_width/2, -mount_hole_offset])
      circle(d=mount_hole_diam);
    translate([body_width/2, body_length+mount_hole_offset])
      circle(d=mount_hole_diam);
  }
}

rim_width = 3;
stand_thickness = 3;
stand_height = 120;
stand_width = 10;

module triangle_cutouts(n=1, length = stand_width + rim_width) {
  l = length / 2 / n;
  for(i = [0:1:n-1]) {
    translate([-stand_width / 2, i * (l + 2 * rim_width)]) {
      polygon([
        [0, 0],
        [stand_width, 0],
        [stand_width, l]]);
      polygon([
        [0, rim_width],
        [stand_width, rim_width + l],
        [0, rim_width + l]]);
    }
  }
}

linear_extrude(height=stand_thickness){
  difference() {
    offset(r=rim_width) {
      hull() {
        servo_mount_cutout();
        translate([-stand_width/2, gear_diameter/2 - stand_height])
          square([stand_width, stand_height]);
      }
    }
    servo_mount_cutout();
    translate([-stand_width/2, gear_diameter/2 - rim_width])
      square([stand_width, stand_thickness]);
    translate([-stand_width/2, gear_diameter/2 - stand_height])
      square([stand_width, stand_thickness]);

    translate([0, gear_hood_diam]) {
      triangle_cutouts(n=2, length=gear_diameter / 2 - 2 * rim_width - stand_thickness);
    }

    translate([0, -(stand_height - gear_diameter / 2) + stand_thickness + rim_width]) {
      triangle_cutouts(n=3, length=stand_height - gear_diameter / 2 - 3 * rim_width - stand_thickness);
    }
  }
}
