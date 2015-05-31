SetOptions[ListPlot, BaseStyle -> {FontSize -> 12}];
SetDirectory[DirectoryName[$InputFileName]];
files = FileNames[];
For[i = 1, i <= Length[files], i++,
 If[FileExtension[files[[i]]] == "csv",
  lbl = Switch[files[[i]],
    "flow_vs_density.csv",
    {"\[Rho]", "q", "Flujo vs densidad"},
    "flow_per_density.csv",
    {"\[Rho]", "q", "Flujo/densidad vs densidad"},
    "flow_vs_rand_prob.csv",
    {"p", "q", "Flujo vs Rand"},
    "flow_vs_vmax.csv",
    {"\!\(\*SubscriptBox[\(v\), \(max\)]\)", "q", "Flujo vs Vmax"},
    "flow_vs_new_car_prob.csv",
    {"\!\(\*SubscriptBox[\(p\), \(n\)]\)", "q", 
     "Flujo vs prob de nuevo auto"},
    "flow_vs_semaphore_density.csv",
    {"\!\(\*SubscriptBox[\(\[Rho]\), \(s\)]\)", "q", 
     "Flujo vs densidad de semaforos"},
    "flow_vs_stop_density.csv",
    {"\!\(\*SubscriptBox[\(\[Rho]\), \(t\)]\)", "q", 
     "Flujo vs densidad de topes"}
    ];
  data =  Import[files[[i]]];
  plt = ListPlot[data, AxesLabel -> {lbl[[1]], lbl[[2]]}, 
    PlotLabel -> lbl[[3]], ImageSize -> 500];
  Export[FileBaseName[files[[i]]] <> ".png", plt];
  , False
  ]
 ]