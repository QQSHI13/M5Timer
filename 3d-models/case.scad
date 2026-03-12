// ESP32 Pomodoro Button Enclosure - OpenSCAD
// Backpack style: Atom Lite on top, extension board middle, battery bottom
// FIXED: Proper manifold geometry

// === Parameters ===
$fn = 50;

// Overall dimensions
outer_size = 28;
wall_thickness = 1.5;

// Height layers
atom_height = 10;
gap = 2;
ext_pcb_height = 2;
component_height = 2;
battery_height = 7;

// Total internal height
total_height = atom_height + gap + ext_pcb_height + component_height + battery_height;

// M2 mounting
m2_hole = 2.2;
m2_head = 4;
m2_nut = 4.5;
m2_post_dia = 6;

// Openings
usb_width = 11;
usb_height = 5;
switch_width = 9;
switch_height = 4;

// === Main Case (Bottom) ===
module main_case() {
    difference() {
        // Outer shell
        linear_extrude(height = total_height)
            rounded_square(outer_size, outer_size, 2);
        
        // Hollow out - make sure we don't create zero-thickness walls
        translate([wall_thickness, wall_thickness, wall_thickness])
            linear_extrude(height = total_height - wall_thickness + 0.01)
                rounded_square(outer_size - wall_thickness*2, 
                               outer_size - wall_thickness*2, 1);
        
        // USB-C opening
        translate([(outer_size - usb_width)/2, -0.5, total_height - 10])
            cube([usb_width, wall_thickness + 1, usb_height]);
        
        // Switch opening
        translate([outer_size - wall_thickness - 0.5, 8, total_height - 15])
            cube([wall_thickness + 1, switch_width, switch_height]);
    }
    
    // Internal shelf for extension board
    shelf_z = battery_height;
    difference() {
        translate([wall_thickness + 1, wall_thickness + 1, shelf_z])
            cube([outer_size - wall_thickness*2 - 2, 
                  outer_size - wall_thickness*2 - 2, 
                  1.5]);
        // Cutout for center screw
        translate([outer_size/2, outer_size/2, shelf_z - 1])
            cylinder(d=m2_post_dia + 1, h=3);
    }
    
    // Corner posts for Atom Lite
    post_height = total_height - 2;
    post_positions = [
        [wall_thickness + 3, wall_thickness + 3],
        [outer_size - wall_thickness - 3, wall_thickness + 3],
        [wall_thickness + 3, outer_size - wall_thickness - 3],
        [outer_size - wall_thickness - 3, outer_size - wall_thickness - 3]
    ];
    
    for (pos = post_positions) {
        difference() {
            translate([pos[0], pos[1], 0])
                cylinder(d=4, h=post_height);
            translate([pos[0], pos[1], -0.5])
                cylinder(d=m2_hole, h=post_height + 1);
        }
    }
    
    // Center M2 post
    difference() {
        translate([outer_size/2, outer_size/2, 0])
            cylinder(d=m2_post_dia, h=total_height - 1);
        translate([outer_size/2, outer_size/2, -0.5])
            cylinder(d=m2_hole, h=total_height + 1);
        // Nut trap at bottom
        translate([outer_size/2, outer_size/2, -0.5])
            cylinder(d=m2_nut, h=3, $fn=6);
    }
    
    // Battery retention clips
    clip_positions = [
        [wall_thickness + 2, wall_thickness + 8],
        [wall_thickness + 2, outer_size - wall_thickness - 12],
        [outer_size - wall_thickness - 4, wall_thickness + 8],
        [outer_size - wall_thickness - 4, outer_size - wall_thickness - 12]
    ];
    
    for (pos = clip_positions) {
        translate([pos[0], pos[1], 2])
            cube([2, 4, battery_height - 3]);
    }
}

// === Lid (Top Cover) ===
module lid() {
    lid_height = 3;
    
    difference() {
        // Lid base
        linear_extrude(height = lid_height)
            rounded_square(outer_size, outer_size, 2);
        
        // Window for Atom Lite
        window_size = 20;
        translate([(outer_size - window_size)/2, 
                   (outer_size - window_size)/2, 
                   -0.5])
            linear_extrude(height = lid_height + 1)
                rounded_square(window_size, window_size, 2);
        
        // M2 screw head clearance
        translate([outer_size/2, outer_size/2, -0.5])
            cylinder(d=m2_head + 1, h=lid_height + 1);
    }
    
    // Press-fit tabs
    tab_positions = [
        [outer_size/2, 3],
        [outer_size/2, outer_size - 3],
        [3, outer_size/2],
        [outer_size - 3, outer_size/2]
    ];
    
    for (pos = tab_positions) {
        translate([pos[0], pos[1], -1.5])
            cylinder(d=2.5, h=1.5);
    }
}

// === Helper: Rounded Square ===
module rounded_square(w, h, r) {
    if (r > 0) {
        offset(r = r) offset(r = -r)
            square([w, h]);
    } else {
        square([w, h]);
    }
}

// === Export Selection ===
// Choose ONE to export:

// Option 1: Main case
main_case();

// Option 2: Lid (uncomment to export)
// translate([outer_size + 10, 0, 0]) lid();

// Option 3: Assembly preview (not for export)
// assembly();

// === Assembly Preview Module ===
module assembly() {
    // Main case
    color("gray", 0.7)
        main_case();
    
    // Lid
    translate([0, 0, total_height])
        color("lightgray", 0.7)
        lid();
    
    // Atom Lite (visual only)
    translate([(outer_size - 24)/2, 
               (outer_size - 24)/2, 
               battery_height + ext_pcb_height + component_height + gap])
        color("darkgray")
        cube([24, 24, atom_height]);
    
    // Extension board (visual only)
    translate([(outer_size - 24)/2, 
               (outer_size - 24)/2, 
               battery_height + 1.5])
        color("green")
        cube([24, 24, 1.6]);
    
    // Battery (visual only)
    translate([(outer_size - 30)/2 + 2, 
               (outer_size - 40)/2 + 2, 
               1])
        color("blue")
        cube([30, 40, 5]);
    
    // M2 screw (visual only)
    translate([outer_size/2, outer_size/2, 0])
        color("silver")
        cylinder(d=2, h=total_height + 4);
}
