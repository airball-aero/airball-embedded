funcprot(0);

raw = csvRead('airball_log_computed.csv');

I_BARO  =  1
I_TEMP  =  2
I_DP0   =  3
I_DPA   =  4
I_DPB   =  5
I_IAS   =  6
I_TAS   =  7
I_ALPHA =  8
I_BETA  =  9
I_QFS   = 10

I_FLIGHT_START =  23000
I_FLIGHT_END   = 107000

function plotXY(x, y, in_xlabel, in_ylabel, fname)
    f = get("current_figure");
    f.figure_size=[1280,1024];
    
    h = gca();
    plot(x, y);
    xlabel(h, in_xlabel);
    ylabel(h, in_ylabel);
    xgrid(1)
    
    xs2png(gcf(), fname);
    
    delete(f)
endfunction

function plotone(x, y, x_label, y_label, bounds, linespec)
    plot(x, y, linespec);
    h.data_bounds=bounds
    xlabel(gca(), x_label)
    ylabel(gca(), y_label)
    xgrid(1)
endfunction

function plotregion(vec, istart, iend, fname)
    x = (istart:iend)' ./ 20;
    x0 = istart / 20;
    x1 = iend / 20;
    
    subplot(4,1,1);
    plot(x, vec(istart:iend,I_IAS), "-b");
    plot(x, vec(istart:iend,I_TAS), "-m");
    xlabel(gca(), "time (s)");
    ylabel(gca(), "IAS/TAS (m/s)");
    xgrid(1);

    subplot(4,1,2);
    plotone( ..
	x, ..
	vec(istart:iend,I_BARO), ..
	"time (s)", ..
	"baro (Pa)", ..
	[x0,60000;x1,100100], ..
	"-k");

    subplot(4,1,3);
    plotone( ..
	x, ..
	vec(istart:iend,I_ALPHA), ..
	"time (s)", ..
	"alpha (rad)", ..
	[x0,-0.3;x1,0.3], ..
	"-k");

    subplot(4,1,4);
    plotone( ..
	x, ..
	vec(istart:iend,I_BETA), ..
	"time (s)", ..
	"beta (rad)", ..
	[x0,-0.3;x1,0.3], ..
	"-k");

endfunction

f = get("current_figure");
f.figure_size=[1920,1080];
plotregion(raw, I_FLIGHT_START, I_FLIGHT_END)
xs2png(gcf(), "entire_flight.png");
delete(f)
