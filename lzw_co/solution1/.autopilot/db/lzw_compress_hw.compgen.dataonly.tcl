# This script segment is generated automatically by AutoPilot

set axilite_register_dict [dict create]
set port_control {
s1 { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 16
	offset_end 27
}
length_r { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 28
	offset_end 39
}
is_dup { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 40
	offset_end 51
}
dup_index { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 52
	offset_end 63
}
temp_out_buffer { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 64
	offset_end 75
}
temp_out_buffer_size { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 76
	offset_end 87
}
ap_start { }
ap_done { }
ap_ready { }
ap_continue { }
ap_idle { }
}
dict set axilite_register_dict control $port_control


