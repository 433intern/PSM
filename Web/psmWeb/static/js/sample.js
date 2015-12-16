var list = [];

function unescapeHtml(string) {
  return $("<div/>").html(string).text()
}

function StrToJsonArray(cpl) {
  cpl = cpl.replace(/'/gi, "\"");
  console.log(cpl);
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


$(function() {
  var cpl = $("#cpl").attr('value');
  console.log(cpl)

  list = StrToJsonArray(cpl);
  console.log(list)

  list.sort(function(a, b){
    return String(a['name']) > String(b['name'])
  })


  var body = $('tbody#processList')

  for (var i=0; i<list.length; i++){
    pids = list[i]['pids'];
    size = pids.length;
        
    var elem = $(document.createElement('tr'));
    var name_td = $(document.createElement('td'));
    var size_td = $(document.createElement('td'));

    name_td.text(list[i]['name']);
    name_td.addClass('mdl-data-table__cell--non-numeric');

    size_td.text(String(size));
    size_td.addClass('mdl-data-table__cell--non-numeric');
    elem.append(name_td);
    elem.append(size_td);

    body.append(elem);
  }

  var checkList = StrToJsonArray($("#checkl").attr('value'));

  console.log(checkList);

  var agent = StrToJson($("#agent").attr('value'));
  console.log(agent)

  for (var i=0; i<checkList.length; i++){
    var processName = String(checkList[i]['name']);
    var modalButton = $('a#modal' + String(checkList[i]['name']));
    modalButton.click({name : processName}, function(param){
        $('#myModal #myModalTitle').text("프로세스상태보기 - " + param.data.name);
        $('#myModal').modal('toggle'); 
    });

  }
});