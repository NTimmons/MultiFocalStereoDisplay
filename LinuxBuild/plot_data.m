%D = dataset( 'file', 'output_distance.csv', 'Delimiter', ',' );

%{'Stereo'}; {'NonStereo'};
% {'Stereo'}; {'NonStereo'}; 

filenames   = [ {'All'}; {'Stereo'}; {'NonStereo'}; {'Rand20'}; {'Rand40'}; {'Rand80'};   {'Nick'}; {'Nick2'}; {'Daniel'}; {'Emily'}; {'Hui'}; {'Kawin'}; {'Muthu'}; {'Leran'}; {'Josie'} ]; %{'Rafal'}; {'Rain'}; {'Shusan'}; {'Youma'}; {'Stephan'}];
alph        = [ {'ALL'}; {'Stereo'}; {'NonStereo'}; {'Random20'}; {'Random40'}; {'Random80'}; {'Personal1'}; {'Personal2'}; {'A'}; {'B'};{'C'}; {'D'} ;{'E'};{'F'};{'G'};{'H'}; {'I'};{'J'};{'K'};{'L'};{'M'};{'N'};{'O'};{'P'}];
sampleCounts = [ {140}; {100}; {40}; {20}; {40}; {80}; {20}; {20}; {20}; {20}; {20}; {20}; {20}; {20}; {20}; {20}; {20}; {20}; {20} ];


for nam=1:length(filenames)
 
    D = dataset( 'file', strcat(strcat('output_distance_', filenames{nam}), '.csv'), 'Delimiter', ',' );
    Dg = grpstats( D, { 'offset', 'blend', 'interoculartype' }, @mean, 'DataVars', { 'correct' } );

    BT = unique( Dg.blend );
    RT = unique( Dg.interoculartype );

    titles = [ {'No Depth Cues'} {'Multi-Focal'} {'Sterepscopic'} {'Multi-focal Sterepscopic'} ];

    index = 1;
    clf
    for ll=1:length(RT)
        for kk=1:length(BT)

           subplot( 1, 4, index);
           thing = strcmp(Dg.blend, BT{kk}) & strcmp(Dg.interoculartype, RT{ll});
           Bs = Dg(thing,:);
                      
           %Bs.offset, Bs.mean_correct,
           
           %standard error of the mean - Standard Deviation / sqrt(Samplecount)
           e  = (std(Bs.mean_correct)*ones(size(Bs.offset))) / sqrt(sampleCounts{nam});
           
                   
           x = [-10,100];
           y1 = [ 0.5 0.5];
           y2 = [ 1.0 1.0];
           y3 = [ 0.0 0.0];
           plot(x,y1, ':r', x, y2, ':m', x, y3, ':m');
           
           set(gca,'FontSize',25);
           
           
           hold on;
           eb = errorbar( Bs.offset, Bs.mean_correct,e);
           
           title( titles{index});
           ylim( [-0.1 1.1] );
           xlim( [-0.5 4] );
           xlabel( 'Dif. in depth (cm)' );
           grid on
           grid minor
           
           if index == 1
                ylabel( 'Ratio of Correct' );
           end
           
           index = index + 1;
        end
    end

    t = annotation('textbox', [0.0 0.75 1.0 0.25], ...
        'String', ['Participant(s): ' alph{nam}], ...
        'EdgeColor', 'none', ...
        'HorizontalAlignment', 'center')
    t.FontSize = 25;

    set(gcf, 'PaperPosition', [0 0 90.5 55.0]);
    set(gcf, 'PaperUnits', 'centimeters');
    
    saveas(gcf, strcat(strcat('UserNew_',alph{nam}), '.eps'),'epsc')
    %saveas(gcf, strcat(strcat('UserNew_',alph{nam}), '.png'),'png')

end


