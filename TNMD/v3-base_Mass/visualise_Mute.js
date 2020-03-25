const m001 = "groupplot_initseed100_C100.000000_mu0.100000_theta0.250000_pKill0.200000_pMute0.010000_pInit500_mGen10000_L20_ite100/__AVERAGE.csv";
const m0005 = "groupplot_initseed234_C100.000000_mu0.100000_theta0.250000_pKill0.200000_pMute0.005000_pInit500_mGen10000_L20_ite100/__AVERAGE.csv";
const m0015 = "groupplot_initseed259_C100.000000_mu0.100000_theta0.250000_pKill0.200000_pMute0.015000_pInit500_mGen10000_L20_ite100/__AVERAGE.csv";
const m0020 = "groupplot_initseed274_C100.000000_mu0.100000_theta0.250000_pKill0.200000_pMute0.020000_pInit500_mGen10000_L20_ite100/__AVERAGE.csv";
const m0025 = "groupplot_initseed319_C100.000000_mu0.100000_theta0.250000_pKill0.200000_pMute0.025000_pInit500_mGen10000_L20_ite100/__AVERAGE.csv";

console.log("BEGIN");
    
    // set the dimensions and margins of the graph
    var margin = {top: 10, right: 200, bottom: 50, left: 60},
        width = 1600 - margin.left - margin.right,
        height = 800 - margin.top - margin.bottom;
    
    // ADD SVG CANVAS
    var svg = d3.select("#my_dataviz")
      .append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
      .append("g")
        .attr("transform",
              "translate(" + margin.left + "," + margin.top + ")");

    var padding = -100;
    // SET RANGES (size of axes)
    var x  = d3.scale.log().range([ 0, width ]);
    var y  = d3.scale.linear().range([ height, 0 ]);


    var axisX = d3.svg.axis().scale(x).orient("bottom");
    var axisY = d3.svg.axis().scale(y).orient("left");
    
    var valueline = d3.svg.line()
      .x(function(d) { return x(d.generation); })
      .y(function(d) { return y(d.Npop); });


    svg.append("text")
      .attr("text-anchor", "middle")  // this makes it easy to centre the text as the transform is applied to the anchor
      .attr("transform", "translate("+ (padding/2) +","+(height/2)+")rotate(-90)")  // text is drawn off the screen top left, move down and out and rotate
      .text("Average Population");

    svg.append("text")
      .attr("text-anchor", "middle")  // this makes it easy to centre the text as the transform is applied to the anchor
      .attr("transform", "translate("+ (width/2) +","+(height-(padding/3))+")")  // centre below axis
      .text("Generation (Log Scale)");


    //Read the data
    d3.csv(m0005,    

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

        svg.append("text")
          .attr("transform", "translate(" + (width+3) + "," + y(data[data.length - 1].Npop) + ")")
          .attr("dy", ".35em")
          .attr("text-anchor", "start")
          .style("fill", "#4682B4")
           .text("Mutation Rate = 0.005");

        });


    d3.csv(m001,    

          function(d){
            return { generation : +d.generation, Npop : +d.Npop }
          },

          
          
    
          function(data) {
        
            
            
            //y.domain( [0, d3.max(data, function(d) { return d.Npop; })]);
            // Add the line
            svg.append("path")
              .attr("class", "line")
              .style("stroke","#BA0000")
              .attr("d", valueline(data)) 
              
            svg.append("text")
              .attr("transform", "translate(" + (width+3) + "," + y(data[data.length - 1].Npop) + ")")
              .attr("dy", ".35em")
              .attr("text-anchor", "start")
              .style("fill", "#BA0000")
               .text("Mutation Rate = 0.010");
            });

      d3.csv(m0015,    

          function(d){
            return { generation : +d.generation, Npop : +d.Npop }
          },

          
    
    
          function(data) {
        
            
            
        
            // Add the line
            svg.append("path")
              .attr("class", "line")
              .style("stroke","#00BA00")
              .attr("d", valueline(data)) 
              
            svg.append("text")
              .attr("transform", "translate(" + (width+3) + "," + y(data[data.length - 1].Npop) + ")")
              .attr("dy", ".35em")
              .attr("text-anchor", "start")
              .style("fill", "#00BA00")
               .text("Mutation Rate = 0.015");
            });


        d3.csv(m0020,    

          function(d){
            return { generation : +d.generation, Npop : +d.Npop }
          },

          
    
    
          function(data) {
        
            
            
        
            // Add the line
            svg.append("path")
              .attr("class", "line")
              .style("stroke","#FEE12B")
              .attr("d", valueline(data)) 
              
    
                

             svg.append("text")
              .attr("transform", "translate(" + (width+3) + "," + y(data[data.length - 1].Npop) + ")")
              .attr("dy", ".35em")
              .attr("text-anchor", "start")
              .style("fill", "#F5BD1F")
               .text("Mutation Rate = 0.020");

              });

          d3.csv(m0025,    

            function(d){
              return { generation : +d.generation, Npop : +d.Npop }
            },
  
            
      
      
            function(data) {
          
              
              
          
              // Add the line
              svg.append("path")
                .attr("class", "line")
                .style("stroke","#790079")
                .attr("d", valueline(data)) 
                
                svg.append("text")
                .attr("transform", "translate(" + (width+3) + "," + y(data[data.length - 1].Npop) + ")")
                .attr("dy", ".35em")
                .attr("text-anchor", "start")
                .style("fill", "#790079")
                 .text("Mutation Rate = 0.025");
      
              });

        // function updateData() {

        //     // Get the data again
        //     d3.csv(FILENAME, 
                
        //         function(d){
        //             return { generation : +d.generation, Npop : +d.Npop }
        //           },
        

        //         function(data){

        //             // Scale the range of the data again 
        //             x.domain(d3.extent(data, function(d) { return d.generation; }));
        //             y.domain([0, d3.max(data, function(d) { return d.Npop; })]);
        
        //             // Select the section we want to apply our changes to
        //             var svg = d3.select("#my_dataviz").transition();
        
        //             // Make the changes
        //             svg.select(".line")   // change the line
        //                 .duration(250)
        //                 .attr("d", valueline(data));
        //             svg.select(".x.axis") // change the x axis
        //                 .duration(250)
        //                 .call(axisX);
        //             svg.select(".y.axis") // change the y axis
        //                 .duration(250)
        //                 .call(axisY);
        //         }
        //     )
        //     };
  


        // window.setInterval(function(){
        //     updateData()
        // }, 100);

        