ancho = 84;
largo_inferior = 64;
largo_superior = 51.38;

largo_bat = 46;
ancho_bat = 16.1;
espacio_cables = 10;

orificio = 4;

grosor = 2;

pared = 1.2;
holgura = 0.4;
piso = 2;
alto_base = 7;

ancho_pilar = 30;
grosor_pilar = 3;
alto_pilar = 20;
grosor_seguro = 3;
ancho_seguro = ancho_pilar - 8;

largo_seguro = 8;
ancho_seguro2 = ancho_seguro + 4;
grosor_seguro2 = grosor_seguro + 2;
largo_seguro2 = 2;

largo_motor = 20;
ancho_motor = 12;
alto_motor = 10;
piso_cuna = 0.8;

alto_bat = 12.06;

centro_x = -ancho / 2;
centro_y = -largo_inferior / 2;

luz_suelo = 2;
z_base = luz_suelo + alto_base / 2;

pcb1_z = 6.5;
grosor_pcb = 1.6;
r_llanta = 16;
grosor_inter = 2;
h_motor = 10;

alto_total_inter = grosor + alto_bat;
z_inter = r_llanta + h_motor / 2 + alto_total_inter / 2 + holgura;

offset_x_pcb2 = -4.11;
offset_y_pcb2 = 0.101;

IDC_x = 45.5 - (ancho / 2);
IDC_y = 40.586 - (largo_inferior / 2);
bat_y = IDC_y - 25.55/2 - holgura - (ancho_bat/2 + pared);

//PCBs
pcb_inferior(centro_x, centro_y, pcb1_z);

//referencia
capacitorAltura(0, 0, pcb1_z + grosor_pcb / 2 + 11 / 2);

//piezas a imprimir
base(ancho, largo_inferior, 0, 0, z_base);
baseIntermedia(ancho + (pared + holgura)*2, largo_inferior + (pared + holgura)*2,  0, 0, z_inter);
baseSuperior(ancho + (pared + holgura)*2, largo_inferior + (pared + holgura)*2, 0, 0, z_inter + alto_total_inter/2 + grosor/2);
seguro(0, largo_inferior/2, alto_base + alto_pilar - 2.5, 180);
seguro(0, largo_inferior/2, alto_base + alto_pilar - 2.5, 0);

//codigo
module pcb_inferior(x, y, z) {
	translate([x, y, z])
		rotate([0, 0, 90])
			color([0, 0.5, 0, 0.4])
    			import("../PCBs/inferior/3D/sumo-PCB-inferior.stl", convexity=10);
}

module pcb_superior(x, y, z) {
    translate([x - offset_x_pcb2, y - offset_y_pcb2, z])
		rotate([0, 0, 90])
			color([0, 0.5, 0, 0.4])
        		import("../PCBs/superior/3D/sumo-PCB-superior.stl", convexity=10);
}

module base(ancho, largo, x, y, z) {
	largo_tcrt = 10.35;
	ancho_tcrt = 5.95;

	cols_x_tcrt = [8, 76];
    filas_y_tcrt = [3.5, 60.5];

	union() {
		difference() {
	    	translate([x, y, z])
	    	    cube([ancho + 2 * (holgura + pared), largo + 2 * (holgura + pared), alto_base], center=true);

    		translate([x, y, z + piso])
	        	cube([ancho + 2 * holgura, largo + 2 * holgura, alto_base], center=true);

			for (px = cols_x_tcrt, py = filas_y_tcrt) {
        		pos_x = px - (ancho / 2);
            	pos_y = py - (largo / 2);

            	translate([x + pos_x, y + pos_y, z])
                	cube([largo_tcrt + 0.3, ancho_tcrt + 0.3, 10], center=true);
			}
		}
		difference() {
			union() {
				translate([0, -largo/2 - grosor_pilar + pared, alto_base + alto_pilar/2 + piso])
					cube([ancho_pilar, grosor_pilar, alto_pilar], center=true);
				translate([0, largo/2 + grosor_pilar - pared, alto_base + alto_pilar/2 + piso])
					cube([ancho_pilar, grosor_pilar, alto_pilar], center=true);
			}

			translate([0, -largo/2 - grosor_pilar + pared, alto_base + alto_pilar - 2.5])
				cube([ancho_seguro, grosor_pilar + 1, grosor_seguro], center=true);
			translate([0, largo/2 + grosor_pilar - pared, alto_base + alto_pilar - 2.5])
				cube([ancho_seguro, grosor_pilar + 1, grosor_seguro], center=true);
		}
	}
}

module baseIntermedia(ancho, largo, x, y, z) {
	z_tope_pilar = alto_base + alto_pilar + piso;
	z_centro_corte = z_inter - alto_total_inter/2 + grosor/2;
	alto_corte_pilar = 2 * (z_tope_pilar - z_centro_corte) + holgura;
	grosor_pilar_en_base = 2*pared + holgura - grosor_pilar/2;
	translate([x, y, z])
		union() {
			difference() {
				cube([ancho, largo, alto_total_inter], center=true);

				translate([0, 0, grosor/2 + holgura/2])
					cube([ancho - pared*2, largo - pared*2, alto_bat + holgura], center=true);

				translate([IDC_x, IDC_y, -alto_total_inter/2 + grosor/2])
					cube([9, 25.55, grosor + 1], center=true);

				translate([0, -largo/2 + holgura, -alto_total_inter/2 + grosor/2])
					cube([ancho_pilar + holgura, grosor_pilar_en_base, alto_corte_pilar], center=true);

				translate([0, largo/2 - holgura, -alto_total_inter/2 + grosor/2])
					cube([ancho_pilar + holgura, grosor_pilar_en_base, alto_corte_pilar], center=true);
				translate([0,-27,0])
					cylinder(h=largo_bat*2, r=orificio, center = true);

			}
			translate([-ancho/2, -largo/2, 0])
			for(px = [9.5, (ancho - 9.5)], py = [15, largo - 15]) {
				motor(px, py, -alto_total_inter/2);
			}
			bateria(x + bat_y, ancho/2 - largo_bat/2 - pared - holgura/2, grosor/2, 90);

		}
}

module baseSuperior(ancho, largo, x, y, z) {
	translate([x, y, z])
		cube([ancho, largo, grosor], center=true);
}

module motor(x, y, z) {
	difference() {
		translate([x, y, -((alto_motor / 2) - z)])
			cube([largo_motor - 1, ancho_motor + 4, alto_motor + 2.2 + piso_cuna], center=true);

		translate([x, y, -((alto_motor / 2) - z)])
			cube([(largo_motor), (ancho_motor + 0.4), (alto_motor + 0.4)], center=true);
	}
}

module bateria(x, y, z, a) {
	largo_cavidad = largo_bat + holgura;
	y_cav_neg = -largo_cavidad/2 - espacio_cables;
	y_cav_pos = largo_cavidad/2;
	centro_y_ext = (y_cav_neg + y_cav_pos)/2;
	largo_ext = y_cav_pos - y_cav_neg + 2*pared;

	rotate([0, 0, a])
		translate([x, y, z])
			difference() {
				translate([0, centro_y_ext, 0])
					cube([ancho_bat + pared*2, largo_ext, alto_bat], center=true);

				translate([0, (y_cav_neg + y_cav_pos + pared + 1)/2, 0])
					cube([ancho_bat + holgura, largo_cavidad + espacio_cables + pared + 1, alto_bat + holgura], center=true);
				translate([0,0,0])
					rotate([90,0,0])
						cylinder(h=largo_bat*2, r=orificio, center = true);
			}
}

module seguro(x, y, z, a){
	rotate([0, 0, a])
		union() {
			translate([x, y, z])
				rotate([90, 0, 0])
					cube([ancho_seguro - holgura, largo_seguro, grosor_seguro - holgura], center=true);
			translate([x, y + largo_seguro/2 + largo_seguro2/2, z + grosor_seguro2/2 - grosor_seguro/2 + holgura/2])
				rotate([90, 0, 0])
					cube([ancho_seguro2, largo_seguro2, grosor_seguro2], center=true);
		}
}

//cubo de altura 11 con dimenciones arbitarias
module capacitorAltura(x, y, z){
	translate([x, y, z])
		cube([5, 5, 11], center=true);
}

difference() {
    union() {
//        base(ancho, largo_inferior, 0, 0, z_base);
//        baseIntermedia(ancho, largo_inferior, 0, 0, z_inter);
//		pcb_inferior(centro_x, centro_y, pcb1_z);
    }
//    translate([0, -100, -50])
//        cube([200, 100, 200]);
}
