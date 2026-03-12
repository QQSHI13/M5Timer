// ESP32 Pomodoro Button - Simple Backpack Enclosure
// Atom Lite on top, expansion board on bottom: snap-fit + magnet mount

// === Parameters ===
$fn = 50;

// Atom Lite dimensions
atom_size = 24;
atom_height = 9.5;

// PCB dimensions (same as Atom Lite)
pcb_size = 24;
pcb_thickness = 1.6;

// Component height on PCB
component_height = 4;  // TP4056, USB-C, etc.

// Magnet mount (6x3mm neodymium magnet, common size)
magnet_dia = 6;
magnet_height = 3;

// Snap-fit on GROVE side (top edge)
snap_gap = 0.2;  // Tight clearance for snap
snap_depth = 1.5;
snap_height = 3;

// Battery dimensions
battery_width = 30;
battery_length = 40;
battery_height = 5;

// === Simple Backpack Case ===
// Just the bottom part that holds the extension board and battery
module backpack() {
    // Base plate with walls
    base_thickness = 1.5;
    wall_height = component_height + battery_height + 2;
    
    union() {
        // Main shell with cutouts
        difference() {
            // Outer shell
            linear_extrude(height = wall_height)
                rounded_square(pcb_size + 2, pcb_size + 2, 2);
            
            // Hollow interior
            translate([1, 1, base_thickness])
                linear_extrude(height = wall_height)
                    rounded_square(pcb_size, pcb_size, 1);
            
            // USB-C opening (side)
            translate([(pcb_size + 2 - 12)/2, -0.5, wall_height - 8])
                cube([12, 2, 6]);
            
            // Switch opening (side)
            translate([pcb_size + 2 - 1.5, 6, wall_height - 8])
                cube([2, 10, 4]);
        }
        
        // Snap-fit on GROVE side (top edge - where GROVE connector is)
        // Add small lip on top edge to hold PCB in place
        translate([0, 0, wall_height - snap_height])
            linear_extrude(height = snap_height)
                difference() {
                    square([pcb_size + 2, snap_depth + 1]);
                    translate([1 + snap_gap, 1])
                        square([pcb_size - snap_gap*2, snap_depth]);
                };
        
        // Center magnet cavity (replace M2 screw)
        translate([(pcb_size + 2)/2, (pcb_size + 2)/2, wall_height - magnet_height - 0.5])
            cylinder(d=magnet_dia + 0.2, h=magnet_height + 1);  // Press-fit cavity
        
        // Small alignment pins (for stability, 4 corners)
        pin_positions = [
            [3, 3],
            [pcb_size + 2 - 3, 3],
            [3, pcb_size + 2 - 3],
            [pcb_size + 2 - 3, pcb_size + 2 - 3]
        ];
        
        for (pos = pin_positions) {
            translate([pos[0], pos[1], wall_height - 3])
                cylinder(d=2, h=3);
        }
        
        // Battery retention clips
        translate([1, 4, base_thickness + 2])
            cube([1.5, 6, battery_height - 2]);
        translate([1, pcb_size + 2 - 10, base_thickness + 2])
            cube([1.5, 6, battery_height - 2]);
        translate([pcb_size + 2 - 2.5, 4, base_thickness + 2])
            cube([1.5, 6, battery_height - 2]);
        translate([pcb_size + 2 - 2.5, pcb_size + 2 - 10, base_thickness + 2])
            cube([1.5, 6, battery_height - 2]);
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

// === Assembly Preview ===
module assembly() {
    // Backpack (bottom)
    color("gray", 0.7)
        backpack();
    
    // Extension PCB (visual)
    translate([1, 1, 6])
        color("green", 0.8)
        cube([24, 24, 1.6]);
    
    // Battery (visual)
    translate([3, 3, 1.5])
        color("blue", 0.5)
        cube([30, 40, 5]);
    
    // Magnet (visual)
    translate([(pcb_size + 2)/2, (pcb_size + 2)/2, wall_height - magnet_height - 0.5])
        color("silver")
        cylinder(d=magnet_dia, h=magnet_height);
    
    // Atom Lite (visual, on top)
    translate([1, 1, 15])
        color("darkgray")
        cube([24, 24, 9.5]);
    
    // GROVE cable (visual)
    color("black", 0.5) {
        translate([12, 25, 8])
            cube([6, 4, 2]);
        translate([12, 25, 16])
            cube([6, 4, 2]);
    }
}

// === Export ===
// Export the backpack:
backpack();

// Or preview assembly:
// assembly();
