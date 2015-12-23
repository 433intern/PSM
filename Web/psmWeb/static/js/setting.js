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
  var mcl = StrToJsonArray($("#mcl").attr('value'));
  var pcl = StrToJsonArray($("#pcl").attr('value'));
  var amcl = StrToJsonArray($("#amcl").attr('value'));
  var apcl = StrToJsonArray($("#apcl").attr('value'));
  var token = $("#token").attr('value');
  console.log(amcl);
  console.log(pcl);

  function deletecounter_event(param){
      function onReceived(res) {
        alert(res);
        location.reload();
      }

      resultCounterList = [param.data.name];
      console.log(resultCounterList);

      $.ajax({
        url : '/../deletecounter/',
        type: "POST",
        data : {
          'csrfmiddlewaretoken': document.getElementsByName('csrfmiddlewaretoken')[0].value,
          'token' : param.data.token,
          'ismachine' : param.data.ismachine,
          'cl[]' : resultCounterList,
        },
        dataType : "text",
        success: onReceived
          
      });
    }

  for (var i=0; i<mcl.length; i++){
    var mcname = mcl[i].name;
    var deleteButton = $('#deletemc'+mcname).click({token : token, name : mcname, ismachine : true}, deletecounter_event);
  }

  for (var i=0; i<pcl.length; i++){
    var pcname = pcl[i].name;
    var deleteButton = $('#deletepc'+pcname).click({token : token, name : pcname, ismachine : false}, deletecounter_event);
  }


  function showmodal(param){  
        var modalbody = $('#myModal #myModalTbody');
        var emodalbody = modalbody.get();
        modalbody.html('');

        for (var i=0; i<param.data.acl.length; i++){
          var cName = param.data.acl[i].name;
          var cCounter = param.data.acl[i].counter;

          var eelem = document.createElement('tr');
          var celem = $(eelem);
          var cName_td = $(document.createElement('td'));
          var cCounter_td = $(document.createElement('td'));

          
          eelem.innerHTML ='<td><input id="pcheckbox_' + String(cName) + '" type="checkbox" class="mdl-checkbox__input"></td>';
          cName_td.text(cName);
          cName_td.addClass('mdl-data-table__cell--non-numeric');
          cCounter_td.text(cCounter);

          celem.append(cName_td);
          celem.append(cCounter_td);
          modalbody.append(celem);
        }

        var addmcompletebutton = $("#addcompleteButton").unbind().one('click', function(){
          resultCounterList = []
          for (var i=0; i<param.data.acl.length; i++){
            if ($("#pcheckbox_" + param.data.acl[i].name).is(":checked"))
            {
              resultCounterList.push(JSON.stringify(param.data.acl[i]))
            }
          }
          console.log(resultCounterList);

          function onReceived(res) {
            alert(res);
            location.reload();
          }

          $.ajax({
            url : '/../addcounter/',
            type: "POST",
            data : {
              'csrfmiddlewaretoken': document.getElementsByName('csrfmiddlewaretoken')[0].value,
              'token' : token,
              'ismachine' : param.data.ismachine,
              'cl[]' : resultCounterList,
            },
            dataType : "text",
            success: onReceived
              
          });
          //open('POST', '/../pchart/', {name : String(data.process), pcl : resultCounterList, token : token}, 'newwin');*/
        });

        $('#myModal').modal('toggle'); 
    }
  var addmcounterButton = $('#addmcounter');
  addmcounterButton.click({acl : amcl, token : token, ismachine : true}, showmodal);

  var addpcounterButton = $('#addpcounter');
  addpcounterButton.click({acl : apcl, token : token, ismachine : false}, showmodal);
});