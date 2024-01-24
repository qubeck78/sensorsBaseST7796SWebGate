$fn = 100;

 
module rounded_box(points, radius, height){
    hull(){
        for (p = points){
            translate(p) 
            union()
            {
                sphere(r=radius);
                
                translate([0,0,radius/2]) cylinder(r=radius,h=height-radius/2);
            }
        }
    }
}

module prism(l, w, h)
{
    polyhedron(
               points=[[0,0,0], [l,0,0], [l,w,0], [0,w,0], [0,w,h], [l,w,h]],
               faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
               );

}




module lcdHolder()
{
//#translate([0,37/2+10,0]) rotate([0,180,0]) color("gray") import("lcd.stl");

    
    {
        difference()
        {
            union()
            {
                //base plate
                translate([-5,-10,0]) cube([110+15,65+15,2]);    
                
                //hood
                translate([-5,65+5,-24.4]) cube([110+15,2,26.4]);
                
                //sides
                translate([-2-5,-10,-30+2]) cube([2,65+15+2,30]);
                translate([110+10,-10,-30+2]) cube([2,65+15+2,30]);
                
                
                //holding rings
                translate([3,3.42+1,-3]) cylinder( h = 3, d = 5.4 );
                
                translate([3.6,61.74-3.42+1,-3]) cylinder( h = 3, d = 5.4 );
                
                translate([108.04-3,3.42+1,-3]) cylinder( h = 3, d = 5.4 );
                
                translate([108.04-3,61.74-3.42+1,-3]) cylinder( h = 3, d = 5.4 );
                
                
                //base
                
                difference()
                {
                    translate([-7,-25,-28]) cube( [ 129, 15, 30 ] );
                    translate([-7,-22,-41]) rotate( [ 25, 0, 0 ] ) cube( [ 129, 15, 40 ] );
                    
                }
            }
            
            //screen cutout
            translate([7.22+6.5,2.66-0.5+1,-0.2]) cube([108.04-7.22-7.22-6.5,56+3,2.4]);

            //sd card hole
            
            translate([ 3.42 + 50, 65 + 5 - 0.5, -9  ]) cube([ 26, 3, 3 ]);
            
            //screw holes
            translate([3,3.42+1,-4]) cylinder(d=2.2,h=5);
            translate([3.6,61.74-3.42+1,-4]) cylinder(d=2.2,h=5);
            translate([108.04-3,3.42+1,-4]) cylinder(d=2.2,h=5);
            translate([108.04-3,61.74-3.42+1,-4]) cylinder(d=2.2,h=5);


            //backplate cutout
   
            translate( [ -6, -10, -27 ] ) cube([110+15+2,65+15+2,2.2]);    

   }
            

     

    }
    
    
}

module backPlate()
{
    difference()
    {
        cube( [ 110 + 15 + 2 - 0.4 , 65 + 15 + 2 - 0.4 , 1.8 ] );
    
        translate( [ 110, 0, -0.1 ] ) #cube( [ 5, 5, 2 ] );
   }
}






lcdHolder();

color("gray") translate( [ -6 + 0.2, -10 + 0.2, -27 + 0.2 ] ) backPlate();





