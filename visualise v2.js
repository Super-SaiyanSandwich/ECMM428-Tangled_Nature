const FILENAME = "groupplot_initseed0_C100.000000_mu0.100000_theta0.250000_pKill0.200000_pMute0.010000_pInit500_L20_ite1000.csv";

console.log("BEGIN");
    
    // set the dimensions and margins of the graph
    var margin = {top: 20, right: 20, bottom: 30, left: 50},
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
    var x  = d3.scale.linear().range([ 0, width ]);
    var y  = d3.scale.linear().range([ height, 0 ]);


    var axisX = d3.svg.axis().scale(x).orient("bottom");
    var axisY = d3.svg.axis().scale(y).orient("left");

    
    
    var valueline = d3.svg.line()
      .x(function(d) { return x(d.generation); })
      .y(function(d) { return y(d.Npop); });

    var ID = 256;
      


    //Read the data
    d3.csv(FILENAME,    

      function(d){
        return { id: +d.id, generation : +d.generation, Npop : +d.Npop }
      },

      
      function(data) {

        var allGroup = d3.map(data, function(d){return(d.id)}).keys()

        d3.select("#selectButton")
        .data(allGroup)
        .enter()
        .append('option')
        .text(function (d) { return d; }) // text showed in the menu
        .attr("value", function (d) { return d; })
      

        var dataF = data.filter(function(d){return +d.id === parseInt(ID); });
    
        // FIND DOMAINS
        x.domain(d3.extent(dataF, function(d) { return d.generation; }));
        y.domain( [0, d3.max(dataF, function(d) { return d.Npop; })]);
        
    
        // Add the line
        


        svg.append("g")
          .attr("class", "x axis")
          .attr("transform", "translate(0," + height + ")")
          .call(axisX);
  
        
        svg.append("g")
          .attr("class", "y axis")
          .call(axisY);

      

        function update(selected){
          var dataFilter = data.filter(function(d){return d.id === parseInt(selected)})

          svg.select(".line")   // change the line
                   .duration(250)
                   .attr("d", valueline(dataFilter));

        }


        d3.select("selectButton").on("change", function(d){
          var selected = d3.select(this).property("value")
          update(selected);
        })
        
        
        
        });


   

        