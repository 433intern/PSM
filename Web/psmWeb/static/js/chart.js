
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

var datas = []
var jsonData;

function DataReceived( data ){
  jsonData = data;
}

$(function() {
  var n=180;

  var mclStr = $("#mcl").attr('value');
  var mcl = StrToJsonArray(mclStr);

  console.log($("#agent").attr('value'));
  var agent = StrToJson($("#agent").attr('value'));

  console.log(mcl);
  console.log(agent);

  for (var i=0; i<mcl.length; i++)
  {
    var o = new Object;
    o.name = mcl[i];
    o.data = [];

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
          'interval' : n+agent.responseTime,
          'mcl[]' : mcl,
        },
        dataType : "json",
        success: DataReceived
          
      });
  }
  RequestData();
  var ajaxInterval = 5;
  setInterval(RequestData, ajaxInterval*1000);

  
  function DrawChart() {

    if (jsonData==null) return;
    if (jsonData.length == 0) return;

    var err= 2*agent.responseTime+ajaxInterval+1;
    var curTime = parseInt(new Date().getTime() / 1000);
    var endTime = curTime-err;
    var startTime = endTime-n;

    startTime += 9;
    endTime += 9;

    for (var i=0; i<jsonData.length; i++)
    {
      name = datas[i].name;
      curData = datas[i].data;

      for (var j=curData.length-1; j>=0; j--)
      {
          if (startTime > (curData[j][0]/1000)) break;
      }

      curData = curData.slice(j+1);

      var cur = 0;
      var time= startTime;
      if (curData.length > 0){
          time = (curData[curData.length-1][0])/1000+1;
          cur = curData[curData.length-1][1];
      } 
      var j = 0;

      while (jsonData[i].data[j][0] < time){
        j++;
        if (j>=jsonData[i].data.length){
            break;
        }
      }
      //console.log(jsonData[i].data[j][0]);

      while (j < jsonData[i].data.length && jsonData[i].data[j][0] < endTime)
      {
        for (var k=time; k<jsonData[i].data[j][0]; k+=1)
        {
          if (k-time > agent.responseTime) curData.push([k*1000, 0]);
          else curData.push([k*1000,cur]);
        }
        time = jsonData[i].data[j][0];
        cur = jsonData[i].data[j][1];
        j++;
      }

      for (var k=time; k<endTime; k+=1)
      {
        curData.push([k*1000,cur]);
        if (k-time > agent.responseTime) curData.push([k*1000, 0]);  
      }

      datas[i].data = curData;

      $.plot("#placeholder"+name, [curData], { xaxis: { mode: "time", }, yaxis: { min: 0 }});
    }
  }
  setInterval(DrawChart, 1000);
});