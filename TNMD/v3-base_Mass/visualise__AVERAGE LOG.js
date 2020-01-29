const FILENAME = "groupplot_initseed100_C100.000000_mu0.100000_theta0.250000_pKill0.200000_pMute0.010000_pInit500_mGen10000_L20_ite100\\__AVERAGE.csv"

console.log("BEGIN");
    
    // set the dimensions and margins of the graph
    var margin = {top: 10, right: 30, bottom: 30, left: 60},
        width = 1500 - margin.left - margin.right,
        height = 800 - margin.top - margin.bottom;
    
    // ADD SVG CANVAS
    var svg = d3.select("#my_dataviz")
      .append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
      .append("g")
        .attr("transform",
              "translate(" + margin.left + "," + margin.top + ")");


    // SET RANGES (size of axes)
    var x  = d3.scale.log().range([ 0, width ]);
    var y  = d3.scale.linear().range([ height, 0 ]);


    var axisX = d3.svg.axis().scale(x).orient("bottom");
    var axisY = d3.svg.axis().scale(y).orient("left");
    
    var valueline = d3.svg.line()
      .x(function(d) { return x(d.generation); })
      .y(function(d) { return y(d.Npop); });




    //Read the data
    d3.csv(FILENAME,    

      function(d){
        return { generation : +d.generation, Npop : +d.Npop }
      },


      function(data) {
    
        // FIND DOMAINS
        x.domain(d3.extent(data, function(d) { return d.generation; }));
        y.domain( [0, d3.max(data, function(d) { return d.Npop; })]);

        
    
        // Add the line
        svg.append("path")
          .attr("class", "line")          
          .attr("d", valueline(data)) 


        svg.append("g")
          .attr("class", "x axis")
          .attr("transform", "translate(0," + height + ")")
          .call(axisX);
  
        
        svg.append("g")
          .attr("class", "y axis")
          .call(axisY);

        });
