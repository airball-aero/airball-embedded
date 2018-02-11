funcprot(0);

raw = csvRead('plot.csv');

// From our original paper, one unit of (alpha / dp0) ratio amounts
// to (approximately linearly) 0.2 radians of angle, which is 11.5
// degrees. So multiplying (alpha / dp0) by 11.5 gives us a VERY rough
// idea of the AoA in degrees. The same is true for yaw.

alpha = raw(:,4) ./ raw(:,3) .* 11.5;
beta = raw(:,5) ./ raw(:,3) .* 11.5;

function plotvsQ(vec, istart, iend, fname)
    f = get("current_figure");
    f.figure_size=[1280,1024];
    
    indices = (istart:iend)' ./ 20
    x0 = istart / 20
    x1 = iend / 20
    
    subplot(2,1,1);
    h = gca();
    plot(indices, raw(istart:iend,3));
    h.data_bounds=[x0,0;x1,2000];
    xlabel(h, 'time (s)');
    ylabel(h, 'center dynamic pressue (Pa)');
    xgrid(1)
    
    subplot(2,1,2);
    h = gca();
    plot(indices, beta(istart:iend));
    h.data_bounds=[x0,-15;x1,15];
    xlabel(h, 'time(s)')
    ylabel('yaw (degrees)');
    xgrid(1)

    xs2png(gcf(), fname);
    
    delete(f)
endfunction

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

plotvsQ(beta, 1, length(raw(:,1)), 'dp0_and_beta_all.png')
plotvsQ(beta, 45000, 55000, 'dp0_and_beta_maneuvers.png')
plotvsQ(beta, 46000, 48000, 'dp0_and_beta_slowing.png')

plotXY( ..
    raw(46000:48000,3), ..
    beta(46000:48000), ..
    'center dynamic pressure (Pa)', ..
    'yaw (degrees)', ..
    'beta_vs_dp0_slowing.png')
