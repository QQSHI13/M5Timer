// ESP32 Pomodoro Button - HAT Style Backpack
// Simple mounting plate like Raspberry Pi HAT or Arduino Shield

// === Parameters ===
$fn = 50;

// Board dimensions
pcb_size = 24;

// Battery dimensions  
battery_width = 30;
battery_length = 40;

// Mounting
m2_hole = 2.2;
m2_post_dia = 6;

// Plate dimensions
plate_size = pcb_size + 4;  // 28mm
plate_thickness = 2;
post_height = 5;
atom_post_height = 8;

// === HAT Style Backpack ===
module hat_backpack() {
    difference() {
        union() {
            // Main mounting plate
            cube([plate_size, plate_size, plate_thickness]);
            
            // === BOTTOM SIDE: Extension board mounting posts ===
            corner_offset = 3;
            
            // Post 1: Bottom-left
            translate([corner_offset, corner_offset, 0])
                cylinder(d=m2_post_dia, h=post_height);
            
            // Post 2: Bottom-right  
            translate([plate_size - corner_offset, corner_offset, 0])
                cylinder(d=m2_post_dia, h=post_height);
            
            // Post 3: Top-left
            translate([corner_offset, plate_size - corner_offset, 0])
                cylinder(d=m2_post_dia, h=post_height);
            
            // Post 4: Top-right
            translate([plate_size - corner_offset, plate_size - corner_offset, 0])
                cylinder(d=m2_post_dia, h=post_height);
            
            // === BOTTOM SIDE: Battery retention lips ===
            clip_height = 2;
            battery_x = (plate_size - battery_width) / 2;
            battery_y = (plate_size - battery_length) / 2;
            
            // Side clips
            translate([battery_x - 1, battery_y, 0])
                cube([1, battery_length, clip_height]);
            translate([battery_x + battery_width, battery_y, 0])
                cube([1, battery_length, clip_height]);
            translate([battery_x, battery_y - 1, 0])
                cube([battery_width, 1, clip_height]);
            translate([battery_x, battery_y + battery_length, 0])
                cube([battery_width, 1, clip_height]);
            
            // === TOP SIDE: AtomS3 Lite center post ===
            translate([plate_size/2, plate_size/2, plate_thickness])
                cylinder(d=8, h=atom_post_height);
            
            // === TOP SIDE: Alignment pins ===
            pin_offset = 9;
            translate([plate_size/2 - pin_offset, plate_size/2 - pin_offset, plate_thickness])
                cylinder(d=2.5, h=2);
            translate([plate_size/2 + pin_offset, plate_size/2 - pin_offset, plate_thickness])
                cylinder(d=2.5, h=2);
            translate([plate_size/2 - pin_offset, plate_size/2 + pin_offset, plate_thickness])
                cylinder(d=2.5, h=2);
            translate([plate_size/2 + pin_offset, plate_size/2 + pin_offset, plate_thickness])
                cylinder(d=2.5, h=2);
        }
        
        // === HOLES (subtracted from union above) ===
        
        // Center access hole
        translate([plate_size/2, plate_size/2, -0.5])
            cylinder(d=8, h=plate_thickness + 1);
        
        // Wire routing slots
        translate([-0.5, plate_size/2 - 3, -0.5])
            cube([3, 6, plate_thickness + 1]);
        translate([plate_size - 2.5, plate_size/2 - 3, -0.5])
            cube([3, 6, plate_thickness + 1]);
        
        // Extension board mounting holes (4 corners, bottom)
        corner_offset = 3;
        translate([corner_offset, corner_offset, -0.5])
            cylinder(d=m2_hole, h=post_height + 1);
        translate([plate_size - corner_offset, corner_offset, -0.5])
            cylinder(d=m2_hole, h=post_height + 1);
        translate([corner_offset, plate_size - corner_offset, -0.5])
            cylinder(d=m2_hole, h=post_height + 1);
        translate([plate_size - corner_offset, plate_size - corner_offset, -0.5])
            cylinder(d=m2_hole, h=post_height + 1);
        
        // AtomS3 center hole (top)
        translate([plate_size/2, plate_size/2, plate_thickness - 0.5])
            cylinder(d=m2_hole, h=atom_post_height + 1);
    }
}

// === Assembly Preview ===
module assembly() {
    // Backpack plate
    color("gray", 0.8)
        hat_backpack();
    
    // Battery (bottom, visual only)
    translate([(plate_size - battery_width)/2, (plate_size - battery_length)/2, -5])
        %cube([battery_width, battery_length, 5]);
    
    // Extension board (bottom side, visual only)
    translate([2, 2, plate_thickness + 0.5])
        %cube([pcb_size, pcb_size, 1.6]);
    
    // AtomS3 Lite (top side, visual only)
    translate([2, 2, plate_thickness + atom_post_height - 1])
        %cube([pcb_size, pcb_size, 9.5]);
}

// === Export ===
hat_backpack();

// Uncomment to preview:
// assembly();