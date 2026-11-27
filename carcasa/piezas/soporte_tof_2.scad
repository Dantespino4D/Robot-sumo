include <soporte_tof.scad>

vista = "ada_pieza";

ada_largo = 25.4;
ada_alto = 17.8;
ada_pcb = 1.6;
ada_comp = 2.9;
ada_agujero_d = 2.5;
ada_agujero_dx = 20.32;
ada_agujero_dz = 12.7;
ada_chip_x = 0;
ada_chip_z = 8.9;
ada_chip_ancho = 2.5;
ada_chip_alto = 4.9;
ada_chip_espesor = 1.56;
ada_conector_largo = 4.25;
ada_conector_alto = 6.9;

idx_ada = 4;

module ada_modelo() {
	color([0.1, 0.35, 0.6])
		difference() {
			cube([ada_largo, ada_alto, ada_pcb]);
			for (sx = [-1, 1], sz = [-1, 1])
				translate([ada_largo / 2 + sx * ada_agujero_dx / 2, ada_alto / 2 + sz * ada_agujero_dz / 2, -1])
					cylinder(d=ada_agujero_d, h=ada_pcb + 2, $fn=24);
		}
	color([0.15, 0.15, 0.15])
		translate([ada_largo / 2 + ada_chip_x - ada_chip_ancho / 2, ada_chip_z - ada_chip_alto / 2, ada_pcb])
			cube([ada_chip_ancho, ada_chip_alto, ada_chip_espesor]);
	color([0.85, 0.85, 0.85])
		for (x = [0, ada_largo - ada_conector_largo])
			translate([x, (ada_alto - ada_conector_alto) / 2, ada_pcb])
				cube([ada_conector_largo, ada_conector_alto, ada_comp]);
}

function ada_y_placa() = boss_d / 2 + 0.3;

module ada_en_pie() {
	translate([0, ada_y_placa(), base_s + ada_alto / 2])
		rotate([90, 0, 180])
			translate([-ada_largo / 2, -ada_alto / 2, 0])
				ada_modelo();
}

ada_ranura = 4.6;
ada_ventana_alto = 10;
ada_cables_x = 7;

module soporte_ada(fijo = false) {
	y0 = ada_y_placa() - 0.05;
	zc = base_s + ada_chip_z;
	caja_soporte(fijo, ada_largo / 2 + caja_holgura_x, y0, y0 + ada_ranura, base_s + ada_alto,
		ada_chip_x - caja_ventana_ancho / 2, ada_chip_x + caja_ventana_ancho / 2,
		zc - ada_ventana_alto / 2, zc + ada_ventana_alto / 2, ada_cables_x);
}

module ada_colocado(s, fijo = false) {
	translate([s[0] + s[2], s[1], z_tapa_sup])
		rotate([0, 0, s[3]])
			soporte_ada(fijo);
}

module ada_placa_colocada(s) {
	translate([s[0] + s[2], s[1], z_tapa_sup])
		rotate([0, 0, s[3]])
			ada_en_pie();
}

if (is_undef(ensamble) && vista == "ada_pieza")
	soporte_ada(false);
