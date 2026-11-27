vista = "pieza";

tof_largo = 25;
tof_cuerpo_largo = 16;
tof_ancho = 10.5;
tof_espesor = 5.5;
tof_pcb = 1.6;
tof_oreja_largo = (tof_largo - tof_cuerpo_largo) / 2;
tof_oreja_ancho = 6;
tof_oreja_alto = 6;
tof_sensor_largo = 5;
tof_sensor_ancho = 4;
tof_sensor_alto = 1.1;
tof_sensor_desde_borde = 6;
tof_oreja_tornillo_d = 2.2;

hueco_x = 10.5;
hueco_y = 5.7;
grosor_tapa = 2;
z_tapa_sup = 46.9;
tapa_ancho = 86.4;
tapa_largo = 67.2;
tapa_z0 = 40.15;

hol = 0.25;
pared_s = 0.8;
base_s = 1.6;
base_centro_largo = 16;
poste_ancho = 4.4;
poste_prof = 4;
tornillo_d = 2.2;
piloto_d = 1.7;
boss_d = 5.4;
boss_h = grosor_tapa;
cable_d = 3.4;

x_huecos = [-33, -6, 21];
y_frente = 27.89;
y_atras = -15.67;

// [x_hueco, y_hueco, dx, rumbo]: 0 = mira a +y, positivo = gira a la izquierda
// orden: AI, AC, AD, BI, BC, BD (cada cara es la otra girada 180 grados)
psi = 20;
soportes = [
	[x_huecos[0], y_frente, 0, psi],
	[x_huecos[1], y_frente, 0, 0],
	[x_huecos[2], y_frente, 0, -psi],
	[x_huecos[2], y_atras, 0, 180 + psi],
	[x_huecos[1], y_atras, 0, 180],
	[x_huecos[0], y_atras, 0, 180 - psi]
];

module tof_modelo() {
	x0 = (tof_largo - tof_cuerpo_largo) / 2;
	color([0.1, 0.35, 0.6]) {
		translate([x0, 0, 0])
			cube([tof_cuerpo_largo, tof_ancho, tof_pcb]);
		for (sx = [0, 1])
			difference() {
				translate([sx ? x0 + tof_cuerpo_largo : 0, (tof_ancho - tof_oreja_ancho) / 2, 0])
					cube([tof_oreja_largo, tof_oreja_ancho, tof_pcb]);
				translate([sx ? tof_largo - tof_oreja_largo / 2 : tof_oreja_largo / 2, tof_ancho / 2, -1])
					cylinder(d=tof_oreja_tornillo_d, h=tof_pcb + 2, $fn=24);
			}
	}
	color([0.15, 0.15, 0.15])
		translate([x0 + tof_sensor_desde_borde, (tof_ancho - tof_sensor_ancho) / 2, tof_pcb])
			cube([tof_sensor_largo, tof_sensor_ancho, tof_espesor - tof_pcb]);
}

module tof_en_pie() {
	rotate([90, 0, 180])
		translate([-tof_largo / 2, -tof_ancho / 2, -tof_espesor / 2])
			tof_modelo();
}

module soporte_tof(fijo = false) {
	T = tof_espesor + hol;
	y_poste = -T / 2;
	x_oreja = tof_cuerpo_largo / 2 + tof_oreja_largo / 2;
	z_tornillo = base_s + tof_ancho / 2;
	alto_poste = z_tornillo + poste_ancho / 2;
	x_poste = x_oreja + poste_ancho / 2;
	z_ini = fijo ? -0.1 : 0;

	difference() {
		union() {
			translate([-base_centro_largo / 2, y_poste, z_ini])
				cube([base_centro_largo, T, base_s - z_ini]);
			translate([-x_poste, y_poste - poste_prof, z_ini])
				cube([2 * x_poste, poste_prof, base_s - z_ini]);
			for (sx = [-1, 1])
				translate([sx * x_oreja - poste_ancho / 2, y_poste - poste_prof, 0])
					cube([poste_ancho, poste_prof, alto_poste]);
			if (!fijo)
				translate([0, 0, -boss_h])
					cylinder(d=boss_d, h=boss_h + base_s, $fn=48);
		}
		if (!fijo)
			translate([0, 0, -boss_h - 1])
				cylinder(d=cable_d, h=boss_h + base_s + 2, $fn=32);
		translate([-6, y_poste - 0.1, base_s - 0.8])
			cube([12, 3, 1]);
		for (sx = [-1, 1])
			translate([sx * x_oreja, y_poste + 1, z_tornillo])
				rotate([90, 0, 0])
					cylinder(d=piloto_d, h=poste_prof - 0.6 + 1, $fn=24);
	}
}

module soporte_colocado(s, fijo = false) {
	translate([s[0] + s[2], s[1], z_tapa_sup])
		rotate([0, 0, s[3]])
			soporte_tof(fijo);
}

module tof_colocado(s) {
	translate([s[0] + s[2], s[1], z_tapa_sup + base_s + tof_ancho / 2])
		rotate([0, 0, s[3]])
			tof_en_pie();
}

if (is_undef(ensamble) && vista == "pieza")
	soporte_tof(false);
