function unescapeHtml(string) {
  return $("<div/>").html(string).text()
}

function StrToJsonArray(cpl) {
  cpl = cpl.replace(/'/gi, "\"");
  data = JSON.parse(cpl);

  return data
}

function StrToJson(str){
    str = str.replace(/'/gi, "\"");
    str = str.replace(/True/gi, "true");
    str = str.replace(/False/gi, "false");
    data = eval("("+str +")")
    return data
}

function prefill(num, value) {
  if(typeof num === 'boolean' || ~~num != num || +num < 0) throw new TypeError(num + ' is invalid')
  return Array.apply(null, Array(+num)).map(function (d,i) { return value })
}

datas = []

$(function() {
  var n=180;

  var mclStr = $("#mcl").attr('value');
  var mcl = StrToJsonArray(mclStr);
  var agent = StrToJson($("#agent").attr('value'));

  console.log(mcl);
  console.log(agent);

  for (var i=0; i<mcl.length; i++){
    var o = new Object;
    o['name'] = mcl[i];
    o['data'] = prefill(n, 0);

    datas.push(o);
  }
  console.log(datas);

  function RequestData()
  {
      var curTime = parseInt(new Date().getTime() / 1000)

      $.ajax({
        url : '/../mredis/',
        type: "POST",
        data : {
          'csrfmiddlewaretoken': document.getElementsByName('csrfmiddlewaretoken')[0].value,
          'curTime' : curTime,
          'agentNumber' : agent.agentNumber,
          'responseTime' : agent.responseTime,
          'interval' : n*2,
          'mcl[]' : mcl,
        },
        dataType : "json",
        success: function( data ){
          console.log(data)
            // do something
        }
      });
  }
  RequestData();
  setInterval(RequestData, agent.responseTime*1000);

  var d1 = [];
  for (var i = 0; i < 14; i += 0.5) {
    d1.push([i, Math.sin(i)]);
  }

  var d2 = [[0, 3], [4, 8], [8, 5], [9, 13]];

  // A null signifies separate line segments

  var d3 = [[0, 12], [7, 12], null, [7, 2.5], [12, 2.5]];

  $.plot("#placeholderMemory", [ d1, d2, d3 ]);

});