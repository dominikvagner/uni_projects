/**    ___ _   __
 *    / _ \ | / /
 *   / // / |/ /
 *  /____/|___/
 *
 * @file index.h
 * @author Dominik Vagner, xvagne10
 * @brief Auto reloading website to present the resutlts for the project.
 * @date 03-12-2022
 */

#ifndef INDEX_H
#define INDEX_H

const char main_page[] = R"=====(
<!DOCTYPE html>
<html>
<body>
<div>
  <h1>Sensor Values:</h1><br>
  <ul>
    <li id="bpm"></li>
    <li id="spo2"></li>
  </ul>
</div>

<script>
setInterval(function() {
  getValues()
}, 1000)

function getValues() {
    var xhttp = new XMLHttpRequest()
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            let data = this.responseText.split("_")
            document.getElementById("bpm").innerHTML = data[0]
            document.getElementById("spo2").innerHTML = data[1]
        }
    };
    xhttp.open("GET", "values", true)
    xhttp.send()
}
</script>
</body>
</html>
)=====";

#endif // INDEX_H