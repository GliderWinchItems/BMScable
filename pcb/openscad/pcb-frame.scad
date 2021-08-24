 /* File: BMScable/pcb/pcb-frame.scad
  * frame for pc board
  * Latest edit: 20210818
 */
 
 $fn=20;
  
module rounded_rectangle_hull(wid,slen,ht,rad)
{
 hull()
 {    
    translate([-wid/2+rad,rad,0])
        cylinder(r=rad,h=ht,center=false);

   translate([ wid/2-rad,rad,0])
        cylinder(r=rad,h=ht,center=false);

    translate([-wid/2+rad,slen-rad,0])
        cylinder(r=rad,h=ht,center=false);
    
    translate([ wid/2-rad,slen-rad,0])
        cylinder(r=rad,h=ht,center=false);
 }
}
brdwid = 76.05;
brdlen = 99.91;
brdholdx = 93.3;
brdholdy = 69.5;
brdholdia = 3.5;

basethk = 5;
baserim = 8;
baserad = 4;

crnr = 7;
   ll = baserim*2 + brdlen;
    ww = baserim*2 + brdwid;

module base()
{
 
    difference()
    {
        union()
        {
            translate([0,-ww/2,0])
            rounded_rectangle_hull(ll,ww,basethk,baserad);
            
            // Lip to strengthen cable connect end of pcb
            translate([ll/2-0.01,-brdwid/2,0])
            cube([4,brdwid,basethk+8],center=false);
      
        }
        union()
        {
            // punch out rectangle below pcb
            translate([-brdlen/2,-brdwid/2,-0.01])
                 cube([brdlen,brdwid,50],center=false);
            
            // JIC base mounting holes
            translate([-ll/2+4,-ww/2+4,0])cylinder(d=3.0,h=50,center=false);
            translate([-ll/2+4,+ww/2-4,0])cylinder(d=3.0,h=50,center=false);
            translate([+ll/2-4,-ww/2+4,0])cylinder(d=3.0,h=50,center=false);
            translate([+ll/2-4,+ww/2-4,0])cylinder(d=3.0,h=50,center=false);    
        }
    }
}
// Mounting post and holes for pcb
module post(a)
{
    translate(a)
    { difference()
        {
            union()
            {
                cube([crnr,crnr,basethk+7],center=false);
            }
            union()
            {
                translate([crnr/2,crnr/2,0])
                cylinder(d = 2.9,h = 50,center=false);
            }
        }
    }
}
module total()
{
xx = -0.5; // Hole centering adjustment    
    base();
    
    translate([.7,0,0])
    {
    post([-ll/2+crnr,-ww/2+crnr,0]);
    post([+brdlen/2-crnr+xx,-ww/2+crnr,0]);
    post([-ll/2+crnr,+ww/2-crnr-baserim,0]);
    post([+brdlen/2-crnr+xx,+ww/2-crnr-baserim,0]);
    }
}

total();