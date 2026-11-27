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

caja_pared = 1.6;
caja_pared_lado = 1.0;
caja_holgura_x = 0.15;
caja_chaflan = 2;
caja_entrada = 0.8;
caja_entrada_alto = 1.2;
caja_cables_x = 6;
caja_cables_z = 5;
caja_ventana_ancho = 10;

module caja_soporte(fijo, cx, y0, y1, z_top, v_x0, v_x1, v_z0, v_z1, c_x) {
	w = caja_pared;
	wl = caja_pared_lado;
	z_ini = fijo ? -0.1 : 0;
	ya = min(-0.5, y0 - w - 0.1);
	yb2 = max(0.3, y0 + 0.3);

	difference() {
		union() {
			translate([-cx - wl, y0 - w, z_ini])
				cube([2 * (cx + wl), (y1 - y0) + 2 * w, z_top - z_ini]);
			if (!fijo)
				translate([0, 0, -boss_h])
					cylinder(d=boss_d, h=boss_h + base_s, $fn=48);
		}
		translate([-cx, 0, 0])
			rotate([90, 0, 90])
				linear_extrude(2 * cx)
					polygon([
						[y0, base_s], [y1, base_s],
						[y1, z_top - caja_entrada_alto], [y1 + caja_entrada, z_top + 0.1],
						[y0 - caja_entrada, z_top + 0.1], [y0, z_top - caja_entrada_alto]
					]);
		for (sx = [-1, 1])
			translate([sx * (cx + wl), y1 + w, z_ini - 1])
				linear_extrude(z_top - z_ini + 2)
					polygon([[sx * 0.5, 0.5], [-sx * (caja_chaflan + 0.5), 0.5], [sx * 0.5, -(caja_chaflan + 0.5)]]);
		translate([v_x0, y1 - 0.1, v_z0])
			cube([v_x1 - v_x0, w + 0.2, v_z1 - v_z0]);
		translate([-c_x, y0 - w - 0.1, base_s])
			cube([2 * c_x, w + 0.2, caja_cables_z]);
		translate([-c_x, ya, base_s - 0.8])
			cube([2 * c_x, yb2 - ya, 1]);
		translate([0, 0, -boss_h - 1])
			cylinder(d=cable_d, h=boss_h + base_s + 2, $fn=32);
	}
}

module soporte_tof(fijo = false) {
	T = tof_espesor + hol;
	cx = tof_largo / 2 + caja_holgura_x;
	zc = base_s + tof_ancho / 2;
	caja_soporte(fijo, cx, -T / 2, T / 2, base_s + tof_ancho,
		-0.5 - caja_ventana_ancho / 2, -0.5 + caja_ventana_ancho / 2,
		zc - 4.5, zc + 4.5, caja_cables_x);
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
