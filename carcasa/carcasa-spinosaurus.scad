ensamble = true;
include <piezas/soporte_tof_2.scad>

// Comenta esta línea para usar soportes móviles (pegados) en vez de fijos
fijos = true;

ancho = 84;
largo_inferior = 64;
largo_superior = 51.38;
largo_pcb_sup = 84;

largo_bat = 46;
ancho_bat = 16.1;
espacio_cables = 10;

orificio = 4;

grosor = 2;
muesca = 10;
muescasIx = [-ancho/4, ancho/4];

pared = 1.2;
pared_x = 0.8;
pared_bat = 1.2;
holgura = 0.4;
piso = 2;
alto_base = 7;

alto_base_S = 5.5;

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

alto_bat = 12;
alto_sw = 1;
ancho_llanta = 6.2;
luz_llanta = 0.3;
z_seguro = alto_base + alto_pilar - 2.3;
alto_soporte = 2.5;

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

ancho_base_sup = ancho + (pared_x + holgura) * 2;
largo_base_sup = largo_inferior + (pared + holgura) * 2;
offset_y_sup = (largo_base_sup - (largo_superior + 2 * holgura + 2 * pared)) / 2;
z_pcb_sup = z_inter + alto_total_inter / 2 + grosor;
z_base_sup = z_inter + alto_total_inter / 2 + (alto_base_S + 2 * grosor) / 2;

IDC_x = 45.5 - (ancho / 2);
IDC_y = 40.586 - (largo_inferior / 2);
IDC_box_x = 2.249;
IDC_box_y = 8.586;
IDC_box_ancho = 9.4;
IDC_box_largo = 26.0;
bat_y = IDC_y - 25.55/2 - holgura - (ancho_bat/2 + pared_bat);
bat_tab_x = -(ancho_base_sup/2 - largo_bat/2 - pared_x - holgura/2);
bat_tab_y = bat_y + (ancho_bat/2 + pared_bat/2);
ancho_total_robot = 2 * (ancho/2 + holgura + pared_x + luz_llanta + ancho_llanta);
echo(ancho_total_robot=ancho_total_robot);

//PCBs
pcb_inferior(centro_x, centro_y, pcb1_z);
//pcb_superior(-50.2201, offset_y_sup - 25.6901, z_pcb_sup);

//referencia
capacitorAltura(0, 0, pcb1_z + grosor_pcb / 2 + 11 / 2);

//piezas a imprimir
base(ancho, largo_inferior, 0, 0, z_base);
baseIntermedia(ancho_base_sup, largo_base_sup, 0, 0, z_inter);
baseSuperior(ancho_base_sup, largo_base_sup, 0, 0, z_base_sup);
if (fijos)
	tapa_fija_completa();
else
	tapa(ancho_base_sup, largo_base_sup, 0, 0, z_base_sup);
for (j = [0 : len(soportes) - 1]) {
	if (!fijos) {
		color([0.9, 0.5, 0.1])
			if (j == idx_ada) ada_colocado(soportes[j]);
			else soporte_colocado(soportes[j]);
	}
	if (j == idx_ada) ada_placa_colocada(soportes[j]);
	else tof_colocado(soportes[j]);
}
seguro(0, largo_inferior/2, z_seguro, 180);
seguro(0, largo_inferior/2, z_seguro, 0);

//ruedas
x_llanta = ancho_base_sup / 2 + luz_llanta;
y_llanta = largo_base_sup / 2 - 15;
for (sx = [-1, 1], sy = [-1, 1])
	llanta(sx * x_llanta, sy * y_llanta, sx);
echo(ancho_con_cubo = 2 * (x_llanta + largo_cubo - offset_neumatico));

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

largo_tcrt = 10.35;
ancho_tcrt = 5.95;
cols_x_tcrt = [8, 76];
filas_y_tcrt = [3.5, 60.5];

module base(ancho, largo, x, y, z) {
	union() {
		difference() {
	    	translate([x, y, z])
	    	    cube([ancho + 2 * (holgura + pared_x), largo + 2 * (holgura + pared), alto_base], center=true);

    		translate([x, y, z + piso])
	        	cube([ancho + 2 * holgura, largo + 2 * holgura, alto_base], center=true);

			for (px = cols_x_tcrt, py = filas_y_tcrt) {
        		pos_x = px - (ancho / 2);
            	pos_y = py - (largo / 2);

            	translate([x + pos_x, y + pos_y, z])
                	cube([largo_tcrt + 0.3, ancho_tcrt + 0.3, 10], center=true);
			}
		}
		for (sx = [-1, 1], yc = [-13, 13])
			translate([sx * (ancho/2 + (holgura + 0.1 - 1) / 2), yc, (luz_suelo + piso - 0.1 + pcb1_z) / 2])
				cube([1 + holgura + 0.1, 14, pcb1_z - (luz_suelo + piso - 0.1)], center=true);
		difference() {
			union() {
				translate([0, -largo/2 - grosor_pilar + pared, (luz_suelo + alto_base + alto_pilar + piso) / 2])
					cube([ancho_pilar, grosor_pilar, alto_base + alto_pilar + piso - luz_suelo], center=true);
				translate([0, largo/2 + grosor_pilar - pared, (luz_suelo + alto_base + alto_pilar + piso) / 2])
					cube([ancho_pilar, grosor_pilar, alto_base + alto_pilar + piso - luz_suelo], center=true);
			}

			translate([0, -largo/2 - grosor_pilar + pared, z_seguro])
				cube([ancho_seguro, grosor_pilar + 1, grosor_seguro], center=true);
			translate([0, largo/2 + grosor_pilar - pared, z_seguro])
				cube([ancho_seguro, grosor_pilar + 1, grosor_seguro], center=true);
		}

		y_borde_rampa = largo/2 + holgura + pared;
		ancho_rampa = ancho + 2 * (holgura + pared_x);
		for (s = [1, -1]) rampa_al_ras(ancho_rampa, s * y_borde_rampa, s);
//		for (s = [1, -1]) rampa_elevada(ancho_rampa, s * y_borde_rampa, s);
	}
}

module baseIntermedia(ancho, largo, x, y, z) {
	z_tope_pilar = alto_base + alto_pilar + piso;
	z_centro_corte = z_inter - alto_total_inter/2 + grosor/2;
	alto_corte_pilar = 2 * (z_tope_pilar - z_centro_corte) + holgura;
	grosor_pilar_en_base = 2*pared;

	muescasIy = [1,-1];
	alto_muesca = grosor + 1;
	z_muesca = alto_total_inter/2 - alto_muesca/2 + holgura/2;

	translate([x, y, z])
		union() {
			difference() {
				cube([ancho, largo, alto_total_inter], center=true);

				translate([0, 0, grosor/2 + holgura/2])
					cube([ancho - pared_x*2, largo - pared*2, alto_bat + holgura], center=true);

				translate([IDC_box_x, IDC_box_y, -alto_total_inter/2 + grosor/2])
					cube([IDC_box_ancho, IDC_box_largo, grosor + 1], center=true);

				translate([0, -largo/2 + holgura, -alto_total_inter/2 + grosor/2])
					cube([ancho_pilar + holgura, grosor_pilar_en_base, alto_corte_pilar], center=true);

				translate([0, largo/2 - holgura, -alto_total_inter/2 + grosor/2])
					cube([ancho_pilar + holgura, grosor_pilar_en_base, alto_corte_pilar], center=true);
				translate([0,-27,0])
					cylinder(h=largo_bat*2, r=orificio, center = true);

				for (i = muescasIx) {
					translate([i, (largo/2 - pared/2), z_muesca])
						cube([muesca, pared + holgura, alto_muesca], center=true);
				}

				for (i = muescasIy) {
					translate([i*(ancho/2 - pared_x/2), largo/2 - largo_superior/2- pared - holgura, z_muesca])
						cube([pared_x + holgura, muesca, alto_muesca], center=true);
				}

			}
			translate([-ancho/2, -largo/2, 0])
				for(px = [9.5, (ancho - 9.5)], py = [15, largo - 15]) {
					motor(px, py, -alto_total_inter/2);
				}
				bateria(x + bat_y, ancho/2 - pared_x - holgura/2 - (largo_bat + holgura)/2 - pared_bat, grosor/2, 90);

		}
}

module baseSuperior(ancho, largo, x, y, z) {
	h_total = alto_base_S + 2 * grosor;
	offset_y = (largo - (largo_superior + 2 * holgura + 2 * pared)) / 2;
	ancho_plataforma = ancho - 2 * pared_x;
	alto_muesca = grosor + 1;

	translate([x, y, z])
		translate([0, offset_y, 0])
			union() {
				difference() {
					cube([ancho_plataforma + 2 * pared_x, largo_superior + 2 * holgura + 2 * pared, h_total], center=true);

					translate([0, 0, grosor])
						cube([ancho_plataforma, largo_superior + 2 * holgura, h_total], center=true);

					translate([IDC_box_x, IDC_box_y - offset_y, -h_total/2 + grosor/2])
						cube([IDC_box_ancho, IDC_box_largo, grosor + 1], center=true);

					translate([bat_tab_x, bat_tab_y - offset_y, -h_total/2 + grosor/2])
						cube([muesca + holgura, pared_bat + holgura, grosor + 1], center=true);

					for (i = muescasIx) {
						translate([i, -largo_superior/2 - pared/2 - holgura, h_total/2 - (grosor + holgura)/2 + holgura/4])
							cube([muesca, pared + holgura, grosor + holgura], center=true);
					}
					for (i = [1, -1]) {
						translate([i*(ancho/2 - pared_x/2), -offset_y, h_total/2 - (grosor + holgura)/2 + holgura/4])
							cube([pared_x + holgura, muesca, grosor + holgura], center=true);
					}

					translate([-ancho/2 + pared_x/2, 15.4 - offset_y, (grosor + 1)/2])
						cube([pared_x + 2, 18, h_total - grosor + 1], center=true);
				}
				for (i = muescasIx) {
					translate([i, largo_superior/2 + pared/2 + holgura, -h_total/2 - grosor/2])
						cube([muesca - holgura, pared, alto_muesca], center=true);
				}
				for (i = [1, -1]) {
					translate([i*(ancho/2 - pared_x/2), 0, -h_total/2 - grosor/2])
						cube([pared_x, muesca - holgura, alto_muesca], center=true);
				}
			}
}

module tapa(ancho, largo, x, y, z) {
	h_total = alto_base_S + 2 * grosor;
	offset_y = (largo - (largo_superior + 2 * holgura + 2 * pared)) / 2;

	botones = [
		[14.709, 4.985],
		[19.709, 4.985],
		[24.709, 4.985],
		[3.279, 77.463],
		[8.279, 77.463]
	];
	tof = [for (cx = [3.708, 47.27], cy = [9, 36, 63]) [cx, cy]];
	x_borde = -ancho/2 - 1;
	usb_x_max = 5.923 - largo_pcb_sup/2 + 9.0/2;
	swd_x_max = 3.383 - largo_pcb_sup/2 + 4.25/2;
	usb_y = 32.284 - largo_superior/2 + offset_y;
	swd_y = 40.793 - largo_superior/2 + offset_y;
	usb_alto = 7.5;
	swd_alto = 6.0;
	acceso_x_max = max(usb_x_max, swd_x_max);
	acceso_y_min = min(usb_y - usb_alto/2, swd_y - swd_alto/2);
	acceso_y_max = max(usb_y + usb_alto/2, swd_y + swd_alto/2);

	translate([x, y, z])
		translate([0, 0, h_total / 2 + grosor / 2])
			union() {
				difference(){
					cube([ancho, largo, grosor], center=true);

					for (b = botones)
						translate([b[1] - largo_pcb_sup/2,
						           b[0] - largo_superior/2 + offset_y, 0])
							cylinder(d=3.0, h=grosor + 2, center=true, $fn=32);

					translate([78.694 - largo_pcb_sup/2,
					           46.127 - largo_superior/2 + offset_y, 0])
						cube([8.5, 8.5, grosor + 2], center=true);

					translate([(x_borde + acceso_x_max)/2,
					           (acceso_y_min + acceso_y_max)/2, 0])
						cube([acceso_x_max - x_borde,
						      acceso_y_max - acceso_y_min,
						      grosor + 2], center=true);

					translate([36.567, -6.527, 0])
						cylinder(d=3.0, h=grosor + 2, center=true, $fn=32);

					for (t = tof)
						translate([t[1] - largo_pcb_sup/2,
						           t[0] - largo_superior/2 + offset_y, 0])
							cube([10.5, 5.7, grosor + 2], center=true);

					translate([74 - largo_pcb_sup/2,
					           6 - largo_inferior/2, 0])
						cube([15, 10, grosor + 2], center=true);
				}

				for (i = muescasIx) {
					translate([i, offset_y - largo_superior/2 - pared/2 - holgura, -grosor + 0.1])
						cube([muesca - holgura, pared, grosor], center=true);
				}
				for (i = [1, -1]) {
					translate([i*(ancho/2 - pared_x/2), 0, -grosor + 0.1])
						cube([pared_x, muesca - holgura, grosor], center=true);
				}
			}
}

module motor(x, y, z) {
	difference() {
		translate([x, y, -((alto_motor / 2) - z)])
			cube([largo_motor - 1 + 0.02, ancho_motor + 4, alto_motor + 2.2 + piso_cuna], center=true);

		translate([x, y, -((alto_motor / 2) - z)])
			cube([(largo_motor), (ancho_motor + 0.4), (alto_motor + 0.4)], center=true);
	}
}

module bateria(x, y, z, a) {
	largo_cavidad = largo_bat + holgura;
	y_cav_neg = -largo_cavidad/2 - espacio_cables;
	y_cav_pos = largo_cavidad/2;
	centro_y_ext = (y_cav_neg + y_cav_pos)/2;
	largo_ext = y_cav_pos - y_cav_neg + 2*pared_bat;

	rotate([0, 0, a])
		translate([x, y, z])
			union() {
				difference() {
					translate([0, centro_y_ext, 0])
						cube([ancho_bat + pared_bat*2, largo_ext, alto_bat], center=true);

					translate([0, (y_cav_neg + y_cav_pos + pared_bat + 1)/2, 0])
						cube([ancho_bat + holgura, largo_cavidad + espacio_cables + pared_bat + 1, alto_bat + holgura], center=true);
					translate([0,0,0])
						rotate([90,0,0])
							cylinder(h=largo_bat*2, r=orificio, center = true);
				}
				translate([ancho_bat/2 + pared_bat/2, 0, grosor/2 + muesca/2 + grosor/2 - holgura])
					cube([pared_bat, muesca, grosor], center=true);
			}
}

module seguro(x, y, z, a){
	rotate([0, 0, a])
		union() {
			translate([x, y, z])
				cube([ancho_seguro - holgura, largo_seguro, grosor_seguro - holgura], center=true);
			translate([x, y + largo_seguro/2 + largo_seguro2/2 - 0.6, z])
				cube([ancho_seguro2, largo_seguro2, grosor_seguro2], center=true);
		}
}

module pulsador() {
	z_tapa_inf = z_base_sup + (alto_base_S + 2 * grosor) / 2;
	z_tapa_sup = z_tapa_inf + grosor;
	z_sw = z_pcb_sup + grosor_pcb + alto_sw;
	z_fondo = z_sw + 0.3;
	largo_total = (z_tapa_sup + 0.6) - z_fondo;
	cylinder(d=2.6, h=largo_total, $fn=32);
	translate([0, 0, z_tapa_inf - 1 - z_fondo])
		cylinder(d=4.4, h=1, $fn=32);
}

A_eje = 2.6;
d_eje = 3.0;
hol_d = 0.08;
hol_plano = 0.03;
c_plano = A_eje - d_eje / 2 + hol_plano;

largo_eje = 10;
offset_neumatico = 1.9;
largo_cubo = largo_eje;
d_cubo = 8;
d_rebaje = 5;
h_rebaje = 1;

r_asiento = 12.7;
ancho_rin = 5;
espesor_llanta_rin = 2;
n_radios = 5;
espesor_radio = 2;
ancho_radio = 2.5;
prof_ranura = 0.8;
ancho_ranura = 1;

r_int_neu = 12.5;
rib_alto = 0.6;
rib_ancho = 0.9;
r_hombro = 1;

module perfil_D(c) {
	difference() {
		circle(d=d_eje + hol_d, $fn=64);
		translate([-5, c])
			square([10, 5]);
	}
}

module rin() {
	zc = ancho_llanta / 2;
	z0 = -offset_neumatico;
	difference() {
		union() {
			translate([0, 0, z0])
				cylinder(d=d_cubo, h=largo_cubo, $fn=64);
			translate([0, 0, zc - ancho_rin / 2])
				difference() {
					cylinder(r=r_asiento, h=ancho_rin, $fn=96);
					translate([0, 0, -0.1])
						cylinder(r=r_asiento - espesor_llanta_rin, h=ancho_rin + 0.2, $fn=96);
				}
			for (i = [0 : n_radios - 1])
				rotate([0, 0, i * 360 / n_radios])
					translate([0, -ancho_radio / 2, zc - espesor_radio / 2])
						cube([r_asiento - espesor_llanta_rin / 2, ancho_radio, espesor_radio]);
		}
		translate([0, 0, zc - ancho_ranura / 2 + 0.0])
			difference() {
				cylinder(r=r_asiento + 1, h=ancho_ranura, $fn=96);
				translate([0, 0, -0.1])
					cylinder(r=r_asiento - prof_ranura, h=ancho_ranura + 0.2, $fn=96);
			}
		translate([0, 0, z0 - 0.1])
			linear_extrude(height=largo_cubo + 0.2)
				perfil_D(c_plano);
		translate([0, 0, z0 - 0.1])
			cylinder(d=d_rebaje, h=h_rebaje + 0.1, $fn=48);
	}
}

module neumatico() {
	rotate_extrude($fn=128)
		union() {
			hull() {
				translate([r_int_neu, 0])
					square([0.01, ancho_llanta]);
				translate([r_llanta - r_hombro, r_hombro])
					circle(r=r_hombro, $fn=32);
				translate([r_llanta - r_hombro, ancho_llanta - r_hombro])
					circle(r=r_hombro, $fn=32);
			}
			translate([r_asiento - prof_ranura, ancho_llanta / 2 - rib_ancho / 2])
				square([r_int_neu - (r_asiento - prof_ranura) + 0.01, rib_ancho]);
		}
}

module rueda() {
	color([0.85, 0.85, 0.85])
		rin();
	color([0.1, 0.1, 0.1])
		neumatico();
}

module cupon_eje() {
	pasos = [-1, 0, 1];
	ancho_c = 34;
	largo_c = 14;
	alto_c = 5;
	difference() {
		translate([-ancho_c / 2, -largo_c / 2, 0])
			cube([ancho_c, largo_c, alto_c]);
		for (i = pasos) {
			c = (A_eje - d_eje / 2) + hol_plano + i * 0.1;
			translate([i * 10, 1.5, -0.1])
				linear_extrude(height=alto_c + 0.2)
					perfil_D(c);
			translate([i * 10, -4.5, alto_c - 0.4])
				linear_extrude(height=0.5)
					text(str(c), size=2, halign="center", valign="center");
		}
	}
}

module llanta(x, y, sentido) {
	translate([x, y, r_llanta])
		rotate([0, sentido > 0 ? 90 : -90, 0])
			rueda();
}

//cubo de altura 11 con dimenciones arbitarias
module capacitorAltura(x, y, z){
	translate([x, y, z])
		cube([5, 5, 11], center=true);
}

angulo_rampa = 22;
alto_pared_rampa = 6.2;
sep_rampa = 0.5;

module rampa_base(ancho, y_borde, sentido, z_punta) {
	largo_total = (alto_pared_rampa - z_punta) / tan(angulo_rampa);
	translate([0, y_borde, 0])
		scale([1, sentido, 1])
			translate([-ancho/2, 0, 0])
				rotate([90, 0, 90])
					linear_extrude(height=ancho)
						polygon([[-pared, z_punta], [largo_total, z_punta], [0, alto_pared_rampa], [-pared, alto_pared_rampa]]);
}

module relleno_fondo(sentido) {
	ancho_f = ancho + 2 * (holgura + pared_x);
	largo_f = largo_inferior / 2 + holgura + pared;
	difference() {
		translate([-ancho_f/2, sentido > 0 ? 0 : -largo_f, sep_rampa])
			cube([ancho_f, largo_f, luz_suelo - sep_rampa + 0.01]);
		for (px = cols_x_tcrt, py = filas_y_tcrt) {
			pos_y = py - largo_inferior / 2;
			if (pos_y * sentido > 0)
				translate([px - ancho / 2, pos_y, sep_rampa + (luz_suelo + 0.1 - sep_rampa) / 2 - 0.05])
					cube([largo_tcrt + 0.3, ancho_tcrt + 0.3, luz_suelo + 0.1 - sep_rampa + 0.1], center=true);
		}
	}
}

module rampa_al_ras(ancho, y_borde, sentido) {
	rampa_base(ancho, y_borde, sentido, sep_rampa);
	relleno_fondo(sentido);
}

module rampa_elevada(ancho, y_borde, sentido) {
	rampa_base(ancho, y_borde, sentido, luz_suelo);
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

//VISTA PULSADORES (5 instalados + 1 flotando)
z_pulsador = z_pcb_sup + grosor_pcb + alto_sw + 0.3;
for (p = [[-37.015, -4.671], [-37.015, 0.329], [-37.015, 5.329], [35.463, -16.101], [35.463, -11.101]])
	translate([p[0], p[1], z_pulsador])
		pulsador();
//translate([0, 25, 52])
//	pulsador();

module tapa_fija_completa() {
	union() {
		tapa(ancho_base_sup, largo_base_sup, 0, 0, z_base_sup);
		for (j = [0 : len(soportes) - 1])
			if (j == idx_ada)
				ada_colocado(soportes[j], true);
			else
				soporte_colocado(soportes[j], true);
	}
}
