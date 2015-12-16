var list = [];

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

open = function(verb, url, data, target) {
  var form = document.createElement("form");
  form.action = url;
  form.method = verb;
  form.target = target || "_self";
  if (data) {
    form.innerHTML = $("#csrf").attr('value');
    for (var key in data) {
      var input = document.createElement("textarea");
      input.name = key;
      input.value = typeof data[key] === "object" ? JSON.stringify(data[key]) : data[key];
      form.appendChild(input);
    }
  }
  form.style.display = 'none';
  document.body.appendChild(form);
  form.submit();
};

$(function() {
  var cpl = $("#cpl").attr('value');

  list = StrToJsonArray(cpl);

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
  var token = $("#token").attr('value');

  var mcl = StrToJsonArray($("#mcl").attr('value'));
  console.log(mcl);
  
  var mbutton = $("#machineCheckButton").click(function(){
    resultCounterList = []
    for (var i=0; i<mcl.length; i++){
      if ($("#mcheckbox_" + mcl[i][0]).is(":checked"))
      {
        resultCounterList.push(mcl[i][0])
      }
    }

    open('POST', '/../mchart/', {mcl : resultCounterList, token : token}, 'newwin');

    console.log(resultCounterList);

  });

  for (var i=0; i<checkList.length; i++){
    var processName = String(checkList[i]['name']);
    var modalButton = $('a#modal' + String(checkList[i]['name']));
    modalButton.click({name : processName, token : token}, function(param){  
        function onDataReceived(data) {

            $('#myModal #myModalTitle').text("프로세스상태보기 - "+String(data.process));
            

            var modalbody = $('#myModal #myModalTbody');
            var emodalbody = modalbody.get();
            modalbody.html('');
            var modalTable = $('#myModal #myModalTable');

            for (var i=0; i<data.pList.length; i++){
              var cName = data.pList[i][0];
              var cValue = data.pList[i][1];

              var eelem = document.createElement('tr');
              var celem = $(eelem);

              var cName_td = $(document.createElement('td'));
              var cValue_td = $(document.createElement('td'));

              cName_td.text(cName);
              cName_td.addClass('mdl-data-table__cell--non-numeric');
              cValue_td.text(cValue);

              eelem.innerHTML ='<td><label class="mdl-checkbox mdl-js-checkbox mdl-js-ripple-effect \
              mdl-data-table__select"><input type="checkbox" class="mdl-checkbox__input"/></label></td>';
              celem.append(cName_td);
              celem.append(cValue_td);
              modalbody.append(celem);
            }

            $('#myModal').modal('toggle');       
        }

/*        $.ajax({
            'url' : '/../process_detail/'+param.data.token+'/'+param.data.name,
            'method' : 'get',
            'dataType' : "json",
            'data' : {
                'title' : "프로세스상태보기 - " + param.data.name,
            },
            'success' : onDataReceived
        });

        for (var i=0; i<list.length; i++){
          if (list[i]['name'] == param.data.name){
            pids = list[i]['pids'];
            break;
          }
        }

        console.log(pids)*/

        $.ajax({
            'url' : '/../process_detail/'+param.data.token+'/'+param.data.name,
            'method' : 'get',
            'dataType' : "json",
            'success' : onDataReceived
        });
        
    });
  }
});