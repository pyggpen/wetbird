// Built on some other projects
// I'm sorry, I didn't record where they were from
// but I am grateful to the code creators that throw this stuff out for all of us to use.

// The DHT22 devices used in this project kept dropping and requiring a hard power cycle to come back
// The ESP family has a big 'ol spur at 319.98MHz, which is just right close enough to 320MHz to kill
// Linear Corporation garage door openers. See where that went?

// Project was abandoned, unfortunately. 


// Wetbird 
// BSW and others
// No rights reserved. Don't mess with the crows.

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include "DHT.h"

// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Replace with your network details
const char* ssid = "somenewtorkname";
const char* password = "somesecurenewtorkpassword";

// newtork, lol. Fuck off ES, you should have caught that. 

// Web Server on port 80
WiFiServer server(80);

// DHT Sensor
const int DHTPin = 2;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);


// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];

// only runs once on boot
void setup() {

  pinMode (0, OUTPUT);
  digitalWrite (0, LOW);
  
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  dht.begin();
  
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
  digitalWrite(0, HIGH);
}

// runs over and over again
void loop() {
  // Listenning for new clients
  WiFiClient client = server.available();
  
  if (client) {
    digitalWrite(0, LOW);
    Serial.println("New client");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n' && blank_line) {
            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            float h = dht.readHumidity();
            // Read temperature as Celsius (the default)
            float t = dht.readTemperature();
            // Read temperature as Fahrenheit (isFahrenheit = true)
            float f = dht.readTemperature(true);
            // Check if any reads failed and exit early (to try again).
            if (isnan(h) || isnan(t) || isnan(f)) {
              Serial.println("Failed to read from DHT sensor!");
              strcpy(celsiusTemp,"Failed");
              strcpy(fahrenheitTemp, "Failed");
              strcpy(humidityTemp, "Failed");         
            }
            else{
              // Computes temperature values in Celsius + Fahrenheit and Humidity
              float hic = dht.computeHeatIndex(t, h, false);       
              dtostrf(hic, 6, 2, celsiusTemp);             
              float hif = dht.computeHeatIndex(f, h);
              dtostrf(hif, 6, 2, fahrenheitTemp);         
              dtostrf(h, 6, 2, humidityTemp);
              // You can delete the following Serial.print's, it's just for debugging purposes
              //Serial.print("Humidity: ");
              //Serial.print(h);
              //Serial.print(" %\t Temperature: ");
              //Serial.print(t);
              //Serial.print(" *C ");
              //Serial.print(f);
              //Serial.print(" *F\t Heat index: ");
              //Serial.print(hic);
              //Serial.print(" *C ");
              //Serial.print(hif);
              //Serial.print(" *F");
              //Serial.print("Humidity: ");
              //Serial.print(h);
              //Serial.print(" %\t Temperature: ");
              //Serial.print(t);
              //Serial.print(" *C ");
              //Serial.print(f);
              //Serial.print(" *F\t Heat index: ");
              //Serial.print(hic);
              //Serial.print(" *C ");
              //Serial.print(hif);
              //Serial.println(" *F");
            }
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            // your actual web page that displays temperature and humidity
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head></head><body>");
            client.println("<style> .bordered {width: 300px; height: 350px; text-align: center; padding: 20px; border: 10px inset yellowgreen;}</style>");
            client.println("<div class='bordered'>");     
            client.println("<br><img src='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAJYAAAB0CAYAAABnjctrAAAgAElEQVR4Xs29CZRs61Xft0/Vqbmrq8fb9/adhzfq6elNmuenAaElJLEcB0tgCwOOERgJAkbGAUOCGRxhjCNjEwSCkJUE5BDbWTF2IiQLAkhIQk/D05vffXfseap5rpP121/t7tN965y697aelVqrVnWfOlN93//897w/75//d38nkJhXp9OJ/NYLBpIMBpKQQDzPG/lOJpOR34kMxE+JeJ47nn0TicTum22ZTEavb9ttH74LEp74ubTE/oC4HydJkUFGPC818h5jD9UvB3rvwvu2XgMRj/GNHz9OPWp8E4zZQCQx5tpBEMhgMNh99/t9YVs/EGn3EvyKfd/bvuzTaDR03/Dxu98PRPr9QILA033Cb28csLrd7qGBFTUwTErSZ1CDfYAKgxFgMahhwNnfACuZTX1LgQW4Dgssz3NDzO8KA4htqVRKvwtvZz/d/yaAZZN9GGDpIzQEJqDkbz3vQKTXG3An+0DF997/8PP/VezjNg5YvgSRjMUNAZJbBZYBxxgrDliJdEqC4cTcMmkEvkgAW90+YwVB/5DA6u0y9kFW4vcYY9t3Bj4FlniSDEQSMTNoIBgJLPGk3VPejWSsZrO5CxrOMQpYMBYvYzY+vX/xCz8YC6xer3dLjBX+4TYYUU/cKMYKi7qbAZaX8r+lwBoMet8UYNkYhVmLbdlsNvLB/M8BrHa7fYMoNFEaxVgKrF//xQ/GAouTRL3QseIYS/UgLjLk+hupfr+OxfdhPetmdCyUtNtnrMShGavf7x4OWCpK0dVG66jpdHp3/Iy5d/cNZCxj2RyMFoWetPuDWMZCx47TsUaJQp3zf/lLPxQLLHaKA9ZB5f0gY6m8jRi0sPI+SocKM5YNahh46FiDRNIBK0joBN3SJ2qvisLRBsZ40TqQbrd9KGA55R9hlBQvEez7ZLufTEd+72H8BElB/XYq/I2fqVRGENeDAXpSTz95GBBf/SAYCyxUIdPTTAzelPL+r375h2/XpJEoq9CUS8DAzdwMsOKswrB+EQbWwEs4YDlVduTAxm8HWIDq9oHV6bQOASxnETvrknuA3fd/Ov1l9PcJjJ9BwgEr4sHK5yYkkL4M+iKDoKef/UFXgoEnfTFgRVh9QSCoQrdlFf7GP/l7LzqwblV5v3mrMCl9hLHqjrcDLJjuWwssNzZMAW4W/t4DFoDCnI8ElgSSDBKSQNmJ+P3FYimasRRYPRlINLBMn7plq/A3P/qhIJpR9oTBQT+FishBX9IJT63CqJdZNaO/R79wVlGUKDQdw/QvYyw+g0RSrZrAg3k8xxy38qlWK+b8OE9Q1K8bSLvdFC6v0B7hLojTUcNWc9R9+9xfxO/yJJC05w/HX/UBYLrvM5n0R25nv4EXSKPTVmAZK4XFnfq6hj6vkfMfeNJotEaOn/fxX/nwiwosgBH9Gg8s/Dhh/Wq/jgWw1D1724zlgHW7rz0dK+rhHAusBBMfzbju/kYzEjpWSgBWtCg0HW2UqBx4A2l0W2OBxd2FgQV76baBSLPZHg2s3/7VH1NgRT1xNuSjTgxjpTx1j0XOjDn4bpexDFijfFmBl5J2n+cWyrg9UWh+ttuDFqLKWYVRwLJJGHn+AMdvPLDc9xHACgaSgrFiRGEcMAFWswewnMMz7Key+7YHYxRjcdl2Gwf6jYzv/e4//wlFRZw4HHlSTMpQSCdqYnzfBm7UHuMZi+MPep3tf4DVGQDruGvEQ+awwDIH6ajxGwvWABcDDuTbE8VISF+8WAdp3O8DWO1BR4E1MmQzDNOEGcscrsZY3S7uqBHA+r2P/eShRGFi0I9lrPiJGw8sU+TD4Le/8aJ1B4R0DgEs/3bd9tyFixWiY0UBy6RBFGMFQdJZdLfxUgWg7/g66jUOWJ0AHSuasQ6KwTDJoBc6qTgCWP/Lv/ypQzGW1+/FAit2YNU5GK+8xw7M/w+A5fuJXeU97MA08JvrJQpYgwGGRzQ04kQpwPJ6JAFEv+IkBozVlXjGsvmLUt4dqEYA63/7jf8mEljh2x11YkSh9LqxwIp/EMcDKzwxYWet6gQKrAxG92087/osSvKQjJVO+7FWYSxj46Tsce/RrBkf+RDx+oNDiMJAegnHWFGiUK3vA5kL9j+M5XTAEcD6/f/xHx4CWCJBt3NIYDlxEuVuYLsB6uBnP0hKV4NKYVE4YpJGMgKRtoFojBzP9y16Ktz+A8lkXEgpSkeNVwUS0u/Z/Y52F/R66DA3uhEAo4fLp8e9j/5eH5wYd0PgBTLw8GO5jIWDb8bb7n+kVajfj3bXeJ/8+D+MtQoNsaNiTRIEksMdMIh2sI2jkmQyXseJ84P1vYS0uPa+J2bo09IL25MUFje2Df9bX1KJvniJAQEVze8Kfw64NWJpnkR+HyS5/u0Da298oq1aNRBwKgzwghN/JZ8KpR1Q8dtGf48OGGcV8n1vwLlcftbBN/dGENrE4ShRrzkWo7wK//q3oxmLA5DRUS59GQSSSfr7gLUvrSJkVdye1Sh6/ahX3xPpegnhc+813B+lWKWdAenAdhVAfckkupLwuvtidAkODdCdAun3UM6hJ/d/wkP0uZgegOupijE6gMzlx1md8Tqo8x9FKdCMv6bMDPfZzZMKjfu468c6QEWEIHQcsMLf7WPt//0T8ToWjBEHLPKBDjJW+GbHM1a8fhQ38ABq4KcOBax0siOJRPcG4IwCFoALB4sRgbBmHLBilfehtz5ujOKApZGgPs7RPfCFwaWcPUwKjLrGQTDus/qERD6MK/fkxhknN6gCf/g7Px1rFY4DVtDt6VMzMpZ0E4w17omKG3RlqnTmEMDqStrviCdd8TwoncHb/9ln4jR1eG97IqH2GDsPgeUejlEhnXGMNG7i7fePDqkFu8CKEmXjHuw44No544AVmcj5f/zuz+zqWGFE2skIycQx1qDTVcayuFJYFwtbcVE/8LDAClLpwwEr3R4C68bsAnQQ56cZnV0AsNDOiFUedOLaEz5uDOIdyENpHsFIMNagS7A0mrHGXZ/7jrT6QrpjFGOZA/vg996/+Z/+UayDdJTybuBBxsNYh1Pe40Vh3BMNY/WT/qGAlUq1xEvgiyM2hUK/94moG2CVHdhu/2tStkd2xX5ghUEWG9IJ5bSPE4ej3T0ivTaMuz+Wt+sOGOaqx507zk/FeaKAY0AKh9z2icN/+3s/G2sV2k2OMkcBFn6UFxNYcbFGgMWw3q7yLl5XfL8lXrJ3e1YhVqfn/DhR7oZxQej4IP1+xjqoD8FYAAvGihKF465vD24Ua0VlsBqwwt/vUwX+3f/8c7E61qhEL1PO1SoZWiYvlo4V724QaaNm3KZVCLCSqY7wGWYqL5lwhTdkqPb66qey75OmrOv3zo0Rlyg4bmJvBlhxVuE4YMXVLISt1ihgWZVUlCg8WOyx+4ChY42j4agfxuC3641dHWuUH8SsiqhY2jgdI04HA1C9RFJ6w3wid/2hp5hQiYhL7VWRkFALZ9B39YsUKWSyvqxvXpF0LiGFXE4ClPFuV3qDgWRSKfHTaWnUaro9n81KOpuVbrsttUZD/5+dmZedncpuXj/ntbfpLviBbFLiAtVROpodH6W8B73+PlG4L0g8zKeKNYCGNQ1RjBsG1EEn9kGlPlyz4Jm74Wash4M/zoClbp4QHe/m6wx/WNxNv9jAqlUbWkKVyRScT26QEPK4eQ+CruQn09Ltt5WZYCofn0ICgPWk0+vK/OycfrI9k8vqfrVGXVJJX4rFotSr5d0CEAdgF9A1gBljxI3BQeCFU7utmCF87l1rbai8x+lY4xjL5up2gWXH3QC6P/j4P4hlLAupRCmPnUZT3Q1RVBpfTBHvAA1T9Sjg3wxjNRvtYbgorZMNsBBPDHh30JUjR49ItV6RVrMpSd+XyWJR0pmM9LpdqTca+n+1VlM9ZqJYlGwmIx2KeINAiBM2G2XJZVJaWMo5qRzmk//NVXMrjHWwGMWKGUxqhBlJjSeMixgdaxywzGq8XWCF9ap9FVb/62/8/bHAipXxrfY+YB2kYm78W8lYmXROKLqs19vqRU54bsLVmvHxeCal2+8p2GC0QqGgb/Zh0K5cuSI7OzvKRLOzs1IqlYailQqaQNrtbZnIZwRdifNXq1X95PhcLjemmMQ5HeOAZzpu1IONuyGKscwFNE4ajQO+uaFGiUJLa7rhu9/7Fz8WCywL6UT9sD7m7tCPNUrHGnfTL7YoTCZS0mq1pNVyHuR0KqcAAiid3kDavYFk8gUFGoBgX/6en5/X96VLl2R7e1tZaGpqSiYmJvRYmMRP9CXh1SWXSSiw2Ifjw4xlleQ3KwoPMlZcBqf5sQ4C66AqcjPuhttlLHNHhIGlYPvEr/1ILLDM3B+lQ0HFOEijRGEcqOxJfbGBtb1V1nFNp/OqExXyk8oiW1tbsrVTk3xxWnKFSWWnzc1NWVlZUWAcO3ZMjh8/rsxTqVR0Gwo/923iLu33JeU3JeE5cRtW3k1PAljjJi1qnExSxEkME4WjdDC2jbNKw/pclHERx1jmbrB9TBx6v/Wr8XWFo4C164Ab4EjpxzLWwTBH+ObVaotNXY4P4t6MjlWt1FUU0ikA0VSanFGwwCzVelvShRk6JOj/a2trsr6+row0MzMjc3NzegxMxoBxHACr1Wr6/+SkL53mknTaZZ1ARCjH5fN5/f9mLMJxotB0oDirMM7zPg5YkSGZYZHxOKswshvQxz8aX2JvfpaRohDG6vdvYKwwFZsMjnoasK6iXviP3A93VSGaxhJ6EQDuJlKiOaymwA4IUbg3PiYm+fr1Jbly5Zq0W12Znp5VJpqZnpV0Nifbla7UGi2pbFdkq7wlnWZHkumkTBWnJDeRk6WrS+TMyGRhUvLFvAS9QJqdpv4/f6QoK0tPS622oeBDVHJuPvkfAEZ6plW3GkiC6u2YEnt+g4HroIMUhwrWa5zEGKe8p3znlhnHqlH5cuSjjWI073c+Gl9iP07xUyqOecX5sbQYAAdjOCY1zB8PGC3SUbSKkxxH57B0cTtyp8iDykrXn5DNneZwEjMyPT0t2UxeWi0mti7Xri7J8eMnlWm+8Y0nZXlpRXWnkydPS7aQl1y2IOVqRVaWVmWnsi2TEyWZX5iThCT1/2w6p1bjtSvXJeF7cmxhUbdXdqpy/MSClEoT8vCDL5PPfOYz8vnPf17e9a53yctf/nLVyxCdiEIAxgvAsQ0m451IdiWd6Ug2jYXpYrIUJzgwuD5hA+0/5QpaHbD2YnuabNjtuCTFGMswGjQDIR2OiOftAYuQz17UYZ8f63f/STxjjQPWOKqNK7FnQFJayeueGBLt9EFOOOCoP4mycGJhxO2GTzaZm6o/JHJyZa0pC4tnZX5uQer1uly6dFl1pUK+pKKsXm/J5OSklpQvLy/LyopjF2PSY8cWdALRo/gekai6WKGg248ePaqW3rPPPqvn5zv2BRhnz56Vp59+WoEEU33jG9+Qy5cvy0MPPaTb+O0ADIWfa6LXIfo5p9Pd1sT3K5LJ9CWdzio4nBPXMbXv48JwfjGY6yCwlKl6LgkgSsey+RvlgKVEn2RHPvW5HVFwa6LaGGtXh1JROQQWmUOhhnl6nt/55fg2RocFVpwf62aAhc8IQBmwdACHdYw9Ly1tb1KaHU+6nf7Qo57Tp9+TlE5EtTr0K/nOxVCpNHTyUdIBx+LiUdWLmDxcC4CLSUd5Z/uFCxcUbE888YRcu3ZNhwNgMXh33nmnXL16Va5fv65MBdA++clP6nXf/e53KzgB06lTpxSUX//61/X4l73sZeq62Ni4JqlUXdLp3m6EwLGVS7Dkd8Bg+4FlccFh2VffJQGM1MFCkzeKkQ4Cy0RaWO8zMXnLwPrEL/3d2+7doKwR0+aI7+P8WAYsu3noylXLo1wpjUmX1FmyGKB61CY6pwxDOH0vJfnpM/L85ety+dJVvdbi4nFZWFiQhJdWpX1zc0c2NjYEf9bp06cllcrJc889p24EWCSbTSuzMZgABAUehf3EiRM6+YCCSX7++efl4sWLeowBC9C99KUvlY9+9KMqXj/4wQ+qz+uzn/2ssuRLXvISBRm+L8AJCAHOyZMndf8gaMpEYSDptPOhOVHY1WN8P727LQ5Y3sAZTzbWBxP9DjJW2J0RBlacRRj2roedoNotaFildIO74bd/Mb6j32EZK04xtIJL7eQDmIYpvgos9CneyYQMCA3RuMKaVwz1iY7ny1Z1INmJKRV1S0tL8txzz+vkliZn5ciRI1IoTCrT8P358+dlZuaIrK6uKmMxialUUpkBFwLbTdyhD7Ed0QfAABznYeLYB4Ah0l73utfJn//5n8tjjz0mr3nNa5S5vva1r8lf/dVfyT333KNiEEsTQHEujgeYsNnxxXlZXCwIvdXMbOe86vtCRUgkdsE1StRpVjQ9SG9Tx9JsMm2hNFrHCs/daOV9TxTe6G74hfjmtuOANc7qiHM3GLDsBwAoA5jqWV5CkinfgUr7OblWhQSJVR+RpKyXO5LOE2rJ64Ssr28owNZWtxQADz74iIKoUW+pYj83d1SPRW+C0Uql4vC4dQWWAcA86fwPe7E/AMHK5DqAg78BioHpK1/5inzHd3yHLC4uyle/+lX9LYhTdC/ELgwHM73wwgv6O86cXpQLFxakOJFRVkMEcz72NY87jBpnFSa1qCI6H2uUjrXLTmOAFRaJNwOsfSGdj//jHziUKBwHrDh3A8AyxXGXsbDth6ASbarG8xRoandPgUW3X1eq1AsSkpmYkbWtsuCvggWmpqYVME984xlVrBcWFnWi6rWmHgOwYDL+RrEm3md/IzL5GwCaLwpDAKCUy2XVlxBhgINjmXBA+453vENB8eUvf1mZ5+6771ZgIG5hSUQvxwIyGAyQwpApX+Seu0/L7MykXoPrIlo0jtnrKPi0vEytQveblbuHjA2gfJqCvEiMdavA2mcV/ubPf/+hgDVOxxrnx9oHLGUsl+rrQJV0LAWoBg5QAAu9i0GmrjA7OSPJVFp9VOhIFy++oDoVOhZAe+qpZxUkfM9kHjmyKHfddZeCA4ba2dnadXwiQnkBPACEawBFn79hK0AGe5lSDnPx95NPPin333+/nDlzRr7whS8oIBCTzzzzjLzqVa/S47g3GBTwADBcEEvXL8s9d52TY0fnVfHHYMgXslrmxblpOJJKo3vh0hkNrBSNeV9kq/BWdSyVUv/q5743NjV5HHDGfT8OWL1eR9IZX4jptXtd7V7COGVyBckVJmRruyyF4qS+ARr9mFptFyZJpMl3T2i/8pSfGboIkmrZfe2rTyhTwFiAqFKuqY7U6TivuvmTTpxYVCAiPmElGIfvARPsAhjRewAyViXH8ZsRhbxRytnGJ8wI8GApzvX444/ruRhoc5jCRtyPS+Hpyfmzp2Rna1OOLS7IG97wBuF+trY2pdfvyJEjcwr8bC6jfbi2tzcllU7K/PysWsCV7R1JJ9OSGOaejVNbDn6vbZC08si5b0a94ufPFexaf7CwVfktBtZAGs2aJtDlcgXNh+p1qcsV8VM5SaWzMjFZknqzJeubZdnc2pF2uyPZXEHm5o7I5PSUVJsNWVlblYvPX1Krq9PpKigunL9bxctjj31Nn/611Q1lqGQyo98juhCZTBIAQTmHWZylmFVnJso2n7ALQAE8vAwYDCRAwYpkH8Qd1iDAMkbknGbtmbOYydJAdrsjhVxGup2WMtxDDz0oZ86cklTak0IhL7Nz01Kp7Egun9Zk1Va75jI01ImMD2twaGD5WiN5OGCNbON0WMay8E0U4uNjhQOpt+qSyaUVWJLwVdT1+ugSFCn4cn15RYqlWVk4sigzc/Oa5rK6vikvvHBJllaW5NmLz8v07JQU8kVll7W1dQXSzLQTZ5OT0zrpy0urCh60EgDDfQGsiQmngHMswABA/A9A2I9zcD72NYWf713yYEbFrCb8qXP2ku6LiwJQw3Bs49ywmcUbGSv1vjdbMugOJJtJKYCPHj0iJ08dl5MnF9WrPzVVkq3tNaFaPF/ISCab1PsAbJSgTU5MSK/Vj2UsU/xHzU/YKrzd+UNHfVGAdTP0GxUuwPFJmES97PiAhysddHvoUzRUS0qhOC3lSkMqlbp+n85inufVfdDpteXEmeOSzqaU6VCkr169ppN7dOGEesOvXVtWVinvVIesVB4GpV0OVrNZV8Zh0gEH4OJ4wGK5V4hE3pwHPYy39T9HH8MdwXdYe4CX/7EA+Y590c9gRXN6WnrNoBdINpWVUrEkvp+Ubq8tU1OTcvc9F+SOO85JaaogiSTLjlQklU7IzOykgqxWq2o1NvHMylY1FlhxD/7NACvOXRT2Y90wx4dlLLMcogAW5yAFWIViVlrdlnQ6WEJQsi9JPyNeMqMVMN2+J6trm7K0vC6VWlNSfk51r5SfliA5kEarKj5tBQPHQNVqzcXhvLTqS6dPn1NWSaeyykaXLl1TXQvwIPKWl68riExPwtrjPPwPmHjBXhoWCgIFCGBBLPI/+wEgPgE2uhpMhRMUZRzAso3zWibBbjJgKisT2ZKUilOqoO+UnQ51553n5KX33ytHj83JuXOnlbXanZpMFLmnvARq0HS1smjQ8WKBFdutBv9VQKJgtCiMmz/XG8I5tXntk06HBZbl80QBKy6kA7Cy+ZS2K9TOcF5SMumCpDJ58ZJpLQZ94onnlbno3keaQavdl+1yRdbXN6Vc3hY/M5DCRFZmpudcADqb00nf3qoqCLa2ysN0mWm9RWKFAIsBByy000akwSYcB/gsaMw2RCEvBhj9hrfpWQAXkFgCIIAEdDAS52Y7x6NnAWQAjljEQgT8kxPT4vV9yWXy6magZzwNShCDj7z8AXnJfXdKPp/R9Ya6vYa0O4R/fBWLHI/yPj99NBZYsUvWAKgBfSuigRU3f5qdAagOtMr8pliFcVU0TEBcEFqDymnX8QRR6Kcy4iezAsZgJ95zc8dlfaMsl65cl6tXlqVWb0m+OCmLx07I/NEZqVTXZWn5qjz37EUVQ6lUWnWcO++4V/WcP//zz+vkcwlcA1TrwB7GWE4RdhkDAMLA48z9voKV7fzNgAEmS4XhGMDEGLDNMlPZn33Z5q45UIChoHNtHKaAfmZqXlq1vupZhQl0tYL0B23J5pIKrDe+8XXyl1/4M/174eiMrK5dl0ajJrNzUwruteUVWZhdjAWWPQhROlbQpzIzGlhx8xde/SzsktC/D8tY4xL14tJmNLjsa6NCdTfgj+r3RC3AK1eXZGllQ5ZXNmWyNKsJeojHJv6odkd1LPSzO+48o10BYTyYhjcBaVZkAARHjizI0089qxYhPc9hElwa7MeEb2ys7bacNoCYtWdKO9stiQ+9yxyqgKRQcDn1gAxGQiQaSJlMXB+I2nvvvVf1Llwb+LrQuwBWp8F9NxRUR48dkVarLs1WVR56+GXyzne+XT7xOx+XD3zvd8srX/mIXLl6Ue938fhRyedzsr68IvlEwbUyUmXoRoDoAgc3vNx+2jiJoPeI4+yQuPnbBdaofK5xwLoZ5Txun3jGcs3LEppFmpBGsyXValP9VAk/I+lMQRqtvn5msjkFFrpYuVLXer5qvS5LK9d1YKzyRi22dE4BBBjQb+g6p7lNw3AJoEJRR/Sxj/mWrPhBveKplCrvKOSIHY41cHFuwIcD85lnn1JRy3fGWKbXcE3zc5mBwHewCJ8AcHtzRx8AwMv5BoO+Oknn5mZkZrakovpnf/Zn5PFvfFX1mfkjs3LHHXfIn/7pZ+XbHn27XH/mkvg0+FVv/NArP/TOm6jSKKt2RXbM65Y+GYKLBSNjXmZVRuV74ccayYaHBdY45d0GeZRlSP6VFi8wsCk8zgPRHhcsgJkuaAyw3epJpdGW7a0d2dqparYnHWC8RFq8ZFKZB488kw9zaJGD71wKllNlOpKlC7Mf12WC0Xd4uXDQlE4w25lsgMV9sy/XMSco13J/N6XdaarPCbAZW4VLtgAp9+OA09e3nZtj8L0xaYDK1Tr2VI/CcICVJop5uffeuyWfz8pb3vqouhoQqc8994wcm12QQj8lvnYyAljDdQQVWM5bT5kk2xVM5LZp5z73SQw2MWYBhXHAiqrkHisKxzHWOOV9HLCUHdJp8dM5ncQBS5Akk5JO5SWRzkl5pybiZySZ8KU38BRY29s7srnlGEsZoOdEG4o3TMQLscRbz0lckfjbsFAV4GhCnVXb+L6CiPQZmArQGDgtFson57Y4n9s+kHqjKrlcVoEJeLiGnZ/7QJcC5NwL27lHgM2EsA2Q8D/379blcZaWG9dAHnn5Q/LEE4/LnXdekB/64Q/K2tqKpgUx4Y2dipT8wo3AgrmG4NIuABoO4vcOgTUEmFp8ET1Ebd7HASuqt8ahgXUzyruK/9AKYAZGGEtFJWKKDnmUZHUD6ULX/YRmL1BV42cLks8VlNVaiMJyRYFVqdVUZ2l33YRhffHJ5GLpwSBMnIkftpsibgPHd/wG2Ar/Ey/Tv5h4zmm6E/vyILh0G8I7aalUd5wXfGg12veW8kJOFq4HQj3sg2hFeSepkOuQLAj4uG/OTxqPn3IWYrvd0lScp595Uq3GRx99k1y447xcuHDOZUJQ0lbvS3KYvrwXoIa5sLIRd+7vga4Atp+xFDTavjD6NQ5YUTr2oYE1Tnk3HUuVxQOprxRHsK0Pc7AqFezTDaSJqGn2pNkdSKVcl/Xtily/tiRrG9sKrHy+IDOIgWJRA78wFkyCc9N8TAetsjCozDdjw8lvAFjE9XgAOBf7oBfxN8CyCh3zvrMfIiuTTUm32wmxTkdBh5hDnAFsgEWSIQAmqE1KDZkXPBTnz19QJgSQAFR9bmRcBD0VtZlMWu6864I8//yzymgf+chH5Nz5s2qMzBZLkmyyUJOJQtOxHJj0PWQuFYfBjTqWrY4aBa1xwIoilkMDK35Jk3HuBjcgAAsxp0zGahOUqtc7Umt15daybsMAACAASURBVPjiKdmuNmR9bUM2SI+pNzV1N5XOa9MOtchoZzSsiuHJZ5LM/2JKalj51PSTYeqJ6T8AC1HI/+hQfAIMmAVQmS5hos6dF52Q3KnusCi2pcBE9CGucFXATgCUt4qvRmNXZBPXpLGIKfIE4x07ZlQcwjhYgd/5ne+V60tX5amnnpCPfexjypTk4M9Pz0iqk9BFBLQPb8A6vAg+yu5J5x5IoKt79TXg3Ye1en233/D3jwvJjQNWlCp0aGCNa8Mzzip0Cjs/3vmCkn5WE/lq1ZYq7U89+awkM3nnafd8Bdbm5pasb+zITqWijJXKOKvKKmIsw5NBgTFcYYKzDHkhDk0kAsQ45Z3UF8DC8YhW+72u0qYp6xuryjSmx3EurV8slRRgmlkx9NZzDL+R713GaEIuX7q+W44PUwEuWMqV+aekMJHTQHmpNCkrq0vygQ98QDMeVLlPZ2RQJ1YYaOpMX1yasn4CItpt9zpaM4Afpxf0JCBRMrBaUFSO+CqrccCKMt4ODaz4ZeP2cuKjrEJuHGAhCpU9/Ky0u10p79Rlp9YUP5mRVi+QdqsjzXZPRSFWIUo+nEEYJZt3jMDLFGEm0dJbrEEH5+c+jHX4JOxy0N3AOdgXcMCEltFpAGUbPiw8/8TskkmXRmwOVtPbOAf5WIgty8cCUADUGQ7EMKn8cf41XYzACxRcLgg+Iffff5/8P5/6j/LAA/crgHH+ArD3vOc94rMWT9uByjGVa3SrjKW1AgPp97ohxgJY5LG5T3VDUOVzCB0r6lCPRL+oIDEHxXtex9mM8ZXO6FjqDxqKQhUJLAZAKZFHLDAtyURatip12VjflM3tiuxUahoPrFSbUtcK556KQiaMp9zifsZM6DL2+8Kps6bvoVeZG8E85dY5BkBZDBGw8TLRz/8EsKu1sk74rkEybN3k/EUDTaHhurxNwYdRnR+tLYX8tDqHYaDiJF56vP8t/R+fFeyFWEShb7Uacu3aVX147rvvPnnDa14ri7PHpEcfWLrf5NJSzBe0A3mjVpFyray94OlMrp+0EweDCbeaBTpYnz7uQz+XqQdh9cF0qCg/1m0Dyy4yyqob58PionFWI8BS5Z+QiudShJutnroREIWNTl+Wrq8qwNKpjMYQiVNXKlVZXduSze1ttdoSvmukxmQYcwEWN/kOEKMeHjMozOMOOI3VbCCtHJ/zcX8GENdopKEKMQZ9WGSEBxugcw7rYMN9AFiXStPRJU/IzGAf9oUBLX5JPj7O0p2dbWUygIanv16vSbPZEroL3nHqgpw+eUruuuNOmZ2fUSaqN2vSabeUsZIJXadVmas36KqOxae2+QxYn9YVUxzUQfkNbLPfOwpYcbQylrFswOJYLe4CcX4ugKW1cyjrra4+xTARbgSUd6zCjfVtSeUmpDgxKcUSacgk6LVUx9ra2VFFmIILy42yIgj0Gsx5S2+xgQoPENuw8sxzDjiNySx/m/OaODQl2xytAIC1ePCWm1+Mc1oiIJOClcp5ARaKOd/txiTbfTl54rxWbANaRJ/eP4tTDnqqY+EgJU2G47QGcnZK44UrK2tS2SnL29/8NnnmyWdkZWlZzpw/LW97y6Ny1z136ncXL1+UUnFiqMwjGgMFI6EwH70zGUgT0IaAZYaNjRfjcKtspQ/yOFFooDkYZDQWGCcM41jNejHU6nW1+NBbGk10AlKNc+oghcfRsXoUbtJIVpMBHbPhloCR0BZMwWYCmHhjK6t2UU/zUDzZJwMGmPmOATQAADRNHabZ2sSEHmfO1bCjVb3lQ6vQwjTch+l0fAJctu0rNBj2xOK3PfjAq2Rry6XhkBLDtfFf4dGHa9iWzTlnKochfmFJLUubnpPNlQ255867NGeec6yuLesxlKW95MGXyV/8yX8ahnK6mu7sPO9OYU94fRm065I0j/2IMn0L0LN/2Jo2wrltURjFVDcjBseBDmDBLDBWu+u84+Z5TyYy6nHHj4VIbDZaquB3yTDt9RWAAAtWarScyAMA4bRh80XxacAyazD8ZAJo7gN2IgvB8tQBE24I0zX5HwbU0Iu6LPrSalNp7bablz4MLMBqGRN8z3eWdZrNFKQ4MSebGy7tGXZyOh1isqU+KFwLhHNgxmbTZaLyPzlgM9PT8oW/+KI8+uY3y6OPPqqhJRywuCgQm+fOnZFjx44qoDhnvV5VoFdrFX2g+u2GHJ0t7QPWQcYaZRWGdbDbBpbpSFE6yjiAxflJABZOwhzpvSXX/gflvdFqSaXckGqzI9euLu/qWGSPuoKKpnrecTfABJVaVUUO4DLmYQL5mwEMA8vidTaA7Ic4ZcDZH0sQi8xiezCWFRQAKADAJ78bpZpJ6/dduMjEYZidLPZo6Sv8RoCLl784MSWDfkaqFR4KV/EDgFR30+INrGTy6h0w0L/m5mYV7IjDtZV1ee2rXidbG5v6UJ2/cFZe+tL7lPWefPIb8rWvf0W+53ver+fB0sR7r9GNDvphS/qdpnRqO5IMlZQdVBUsJHer4nCsKLRBvV1g2Y2NQjbAYkLo/Uk2AzePiCtXq7vAIlbY93xhNXeA5aepkiFG11GrECckwFIxGnKMmo6AqW807sTA3mqiXA8waAflTGa3pxXbzC3APqbUW0qxuSfMQ64ZncO+pgeBC6A4h4lqYysNWitjHZFuJxgyWVbFHOLOee+LCizCPIR4KDwBJABQ8/tXVuXb3/5OdZnAtABmfWNNx3RhgSrro/LEk4/reaj4mZufGeb7O1/eoNOSp7/25UhgqfN66Of6pgPLQjZRVuHBnlUHARQHLDLdSzPTyjZLy2uauwQT4ZEmYzSRzGj77HK1LjvlmuZcUb0zMVHU8jCU9mx+QrsYI8osTsdkwl58wjhxg0IYCCcrsTwmBE+7smgut1v/Z45Rfos+6cMMBZRrFG2zCk0XszJ5Js+C0wDF3mZ5euLLRI4wkhOPOSp2tHCjLrk8qTMExZOqPyESn3r6CfW+MxeuacmcuioWFo7JyZPH9X7x1tdqZri09LeVpigKmdWHkIeB+0MctusVWb96UXz8WuHsCFsDaZh1YeO323xkNy3HOpfdSBtjGSsOWASRtUY3xsPGYCMa7B1mD9Jdvvb1r2ufqsnitILA97PSIRGv0ZRmoytf+uJj2uuq0xMFFCGQQqGoeVm1ZlMaMEXfpQ1bOoulxJgVZ0+esZU9JNwLT7r9bwwWdooCqujXYNePZcxuTGnXNr3KGuqaI1UzVLt9OTp3Qk6fPqsAJPkPsQybAQTATUiIAltAee36FWUcWiSxP41Qzt/lqq4R5daEl0ofWilRkU3PLtQB66pjfSQAcrO2La988C4ZtGr6UF+/flV7bU2WJrSCaH11WU4eP+7GtlmXTgunc1d9apkUqU6+1JuNkfmnY4EVJwoB1EDL4aOH3hysowaeZh9+hlCNp5XKWEdraxuys12WdsulK8NS3Y5b0HGyOCVTU7M6yPRv367WpNHrS4t3q6WMFU7iY8DDOqIFpq10y+KEDNye09I1pzXxxVMeBywYyxyk5loJi1zOzXUQp9b4w5gNYM1NH5Hjx07slt7DtCYuuXc63BCWgqEs3sgDBIi2trfle773b2tqN3pnuYyeSQFtRhYWjqo+xufa2qrWAqTTKdnZKcvs7IwGv1OJgXz2//438ppXPiAn77hDqmur8uTj35Cd8pYUyQWbyEuzWlErkjWT0PX6PVKOOrrqGO7sNL0lRsz/TQHLXAsH9aybBZaxlSn6u8r0YCDpXE4azbaKn8uXr2pLR4BFfyuakWmRRUBHmKwUJ0oyMVHSSd/ZrshmuSLZyZJQZ2Lxv3AWgzpch2nDHGPAgoV4A1CsK8DI9XG2Ak5eJrbijRNr87jXeCzMyKozDsNDlhPP+XZdH72BFPMlmZ89okxkIscAaCEnkgER2ZoJMpQApNs8+NBDsrq1oWlHZMniROVhxFqlVYGtWkZMc3HxhJaWwf7oW6dOnZFOuypHpwsyPVVQBiIFqFauqD43gKU6Talsc12RtDXF01BUW4ttAVi+SCjtxi4NY4EV524AWDSYjWMsBtH8OAefaEJcV64tSaOF26DhMkA75AwR6sKCEXWM6jK1CV9TjjMZ4myiTT4qjaZs1erSpcduKJEvbKEBoPB3ZqUa2JkgwAQDWFkW35nosiD1aNYaDyzObblZxtr2YAX9QArZohQLEzI5OaU6Frn6AIQHCktwY2NLrU/0qZmZKd3u+k3ktKXlxStXJUVscaIkM3PTMjM1qwXApGN1+x1ZmD+qsUFaXFJ/WS3XtMCX7dvb6/Kub3+rdNp1zYJQXTKfk069Ik898aQ8+/QTkk1nxGdpYy8hftKT5LD7HyEk3CEyoGrpNoAV5yDVRL0xwIrVsQaBrKztSIviiMBzyXYz8yr2Nje3NVWGF4OJToVfL5FI6kSxP4BqdAMVhVaprH6aYYsiAHbu3LndTNFwBU7YNbA70cN1/8I6ofmmooCFg5SqZPPYm45l4pD7Cj9Y4ZiiF3hSyBYk7aeGgWg871i9VAi5HPZyuSrHjx9T4Dk/VF2Zgh4WWzubMr9wTBtkpeldUczL1OS0ZCkZ83wF1Ctf/iot7C3kJqTRqsvm+pYcO35UTiye1OMffuQBlww4IP+rLf1uW3KZjOpWxdmS/OVn/0T63ZYyFFYnYSB+QzqVlJTvSadR1XaTB19jGSsupHOrwLpRFBJcLWitIBTu8rwnFCRXLl+Va9eWNDNAn96mq+kbDNwaevpkp9KSLsSLQvMpWQDaZRXsOTQptgjrQBbCMLfBOFGIg1QbENI0f+hRN2OBTybLth+MXvDw59JZbaRrehX7cH+wBz41zgt74k7B+cn9kzhoZf2l6UlpdQGaY3zGyLz/Fj0gyfDVr361lsPxu8gV47snn35KfvO3flMWT5yQ17/2tZqheu7MWalWduTalUuytbEh9959p7SadalVympJdlttBbyuLeSLpKRze8CKC0LfKrAOisL+wJNcfk76/aQrwKxUVM+B/p3Ds63WD6Vee55z93SotepnZKNcl0HCTaqByP7mk94JFqDmk22mrFt+VFgHZODDViGTHKe8A6w4xjLrMMz8dj2N27n+l7s5Wcaq6H64GbDquGdcCQAEixHxTNEtetfZ82eVcfgtGoUYFm+wPxKAY2ln+ba3vU1dD4AdNwbX+cu/+pJkChPyf/3RH8l/+Pd/JJVKWe656w554xtfL9/29rfKww89KE98/XHpoVO1XOFIj4d72II9GXQklxpIUgs39r/GMlZc2syt6FgjrcIBK8JPStInYl/XJ5ImshSeok8xMKZEW4Id4RwTYwBqYnpeIDzLZrA0XwuxMNCwAU8/n7zYxzJNaTRrVlo4IyIcuhlnFRqwbtAhhyVWYT+a89i7NBoakbOMHRkJ6TT6la8BaVKdz5w5K/fd9xKZnp6Rz3zm03LlylU5deqkTE6WZHl5SdcM4H/K7/MTOSnk8gT/VBeamZuVC+fOy/GTJ+TlDz/ierb2B/L8Cxdl6dp1OXfhvJw8fkKuLC3L5aVVmZydlvkZOkzX5GtffUxeeOF5ObV4TB5++CEpFYg8sFQMyZIJSenC2WSm0qayLX6/ORpY4zr6hQERZgLVJRKeNLudWOXdrCSONW8253QR/oSsrbVkZXVHn0AYC8XVFkgykeJEH28XprHt3cCTnscq9i5gbA7MsFgwccJxxnr8jelu5Vp8cg1ECYwJE1hvBrYZkLlv88K7c3U1zEKs0JgtnHvFPZk4CvvQdsWzl5AODtcuizpxP/RCxW1SUyvurrvukD/+48/ohJIMePr0Sbn77ns1VvjEE0/JF7/0OSkUsuoAnZ2alkw+J4VsTmbm5+T8mbOysHhMV2AtE5nY2JTtSllXC8NvOFWclOxEUU5cuEO+8OXH5MnHn1T/1Stf/oicP39WWrW6XL16RU4cXdAgeLGQ12rtTNJZnb0OLoiW5JP9aMaK4fp9fqBR7oZWDy959BkAg2UTWAYmAw4V026qUqGznqN2WEQzKDVtlwTAYfwsSVjDNStjuynbXUIyXkaredhmsTyAZenJTCji1Or+7HgTRyj6VjAKS1kJGWBiH4shmnJuOqdzwQSSy7MMyl6c0EbClHe7nrlszEJUH5qXkGzalzaxwb7sNqCDEWhiwrlxqzAO6JiEd47MH9UAM8vlraxek0nYaiKj1lur05by9o4G5dGByKx966NvkWa7Ja1GU1c5U0OhNCkzU9MKrGqnr9Y193Lq1Al5yT13y/R0SXa2t2V1eUmdpPlcVkoTBRckT6ckwRywcESvIynpSnJo9IRR4P3Gf/u3b2pZuVFuBy04HQMsFHJLObELM2muaNSTXPakrK5W1eurpr1Hi2cYiGwHltV1jSdI/1X2TOz1NO8ii1MFZSxenNeS6Cy8w7WtZtD6KAA6K27F42ysbHlSgMKxZEr1FqtRNGWcB8VlMBB2soLQ/dXYxlDhkJaJQTs3SXhJCaTTZtk7yvTpFTGr5fYwFzWEDz74sCb3YSnjfsBomZ2dViuRjsdve+sbNBbIuopk4lbKZdmmzRKJiUEgf/ypT0kbnxQFtjQ0IXmP/H2WzeM3lEqSK07K3PSMTM+UFHgo6zubm1Ipb8udF85JLpOWovaWmNCVZbXQQ9cD70mCB3CUu+HX/9HfigWWAYqJs6fWtjGv7b5LDY56wRQHgcVgu3wpkUL+rKyt1bQUnipf58jkvskYwNqp7YILfwnAciZ9XwGVzJQ0+8H0m4PKN6DQtF+yM4elXThDLRsi7EDlvIAM0Wi1hNalj+M5t6b5dLsqSvFMb21vaG8rE7Uaqgkth2v6VTjLY1fH8ljIsqsNfi1VmaIJFHfOQz4/7hL0QBTwg+0AaPO4vbkiU1MTMj8zqyKOeB4tNzWul/DkyOyc9mztd7q6nfUN+V8954mErGxuyYBWRCzD3G5Kk4euXlO3Ay6I48eOKavmsxll0GwKaaITMASXS3O+QXn/2E9/z9g1oc1cHgWsrpYZRQOLCTqYTrIrCptJ2dlOy/q667FerVZ2QYT+QrjEpzFbqDZur4HFQAaJlHQD3A6uesZ0MBNDTDBsZdu1vdEwndlVHrtSehPVMImJTUsphvks8c9acgMCl26c08IGQihhixNQmJcfEHE/B/PAVF9F7PuiLMD/3B/HEUbigeRv6hDfPMy3AnAWB9XjpadrJmbTLnZHq01XLEHfK7d0zNXLV3Q7TEQ1E4tikcqN6OsnRHITBXVXADysS5iM38PiTeTJr64sSYrOOSjufkJ8D121J8EwUyQxcH03bgDWr/3U+8YCy/SDUTqWLvQdAyxLRzFFNhyuaLd8WV9jncC2BmBhJ7Wwkoi1jnS6rOecdhmQA5igoyBDt1HdJZmWMuVP6pF3rRtNDwunx1i8EDFpVqOxiq1FyP7WexRrFIBZyIfvACLWKixiy8qhbJcrrJVT2M2igAG18duwjZEB19JqwswFsPrdpgLLFnACOIAQXxPuAR64Bx98UBmL+zOG1RBRYiBeryHBwDUZsQc4rM9ZurWxpOl8auxIX9O7ASJedR6m6cmiNn/rNpqaplOkN2yC3Hi3do43TBEawH7aV4Qg/Qhg/epHvisWWKP8WAawm/FjWaqwAcuqXNRX1PKl056VrU23gBF+EiwgREuvT9OOlrJWj/TfPrGptopIRKVah8m0NLvJGxgrnM3AeS0AzDCYG8FihwAahuF/6ybDJFtZmy3SZLFEQMW+7py+Zjeg45iYstAQ19I6yWF1TjiUZJOMjlXeXpNBv6ttLQkvASzcLkwyYCKdB8cmfikDjxXQeiyWngokoDhiqKqEM2jZZkUgdqyVvjkG7WoBhuZ8JV2g3MdB22tLu1FX0ThdnKRWaxdY6megAEPrF2l0knKVVQde3j/9yf8yFljxfiy33oamzbCunjKXLoYzvMxwZYlhlxVnEREAwCNNFmNSuu0Z2dnpDZuhuYJOP+Va+sBYKKxqzvdICaadUE8Sw6YZiaQvfn5K87YYUHNFWGyQewcQpnxbNqexAgCx0nluGHDYus8mwgAK4pPzwHZch0lHVKG8r62v6N+ahtJsKvOyvyn4Fis1xd1ArpOYFOk3q1Le2VBnKHnqHPelL7le8QALsE1POwblfrmv3UqkZlVNfuoDdbE0LGdEnATqZkCnyvgpnR+2U7KTwpnsJ/WTtRq6bX6T0xF13CgQSXoyQSldPifbW1tulS/1kQ2D7ZSTCSoKuV2u78YNwProT/wXNwWsUco7Dc+CBMo7nEizf0wk+g4ALvfZ6fdkoljSMiZ6jaJH9YK2LjUyNbUolXJarl/fVsZiMNFpmBhyfnhKYQWsRSYYYJmoAxz1VlPbS+LjYTuTyCRzHJPsmq25/qKcg6JRgMsksh86E4l9sAIrc3GO973vfSrqPvWpT6l44zwwGPdmFTewGNfCoqR9tmWWcu+8w/lgnMvEtBkY5g/LExNpV+TyxcvyoQ/9gPzKr/yWfP7z/1GbgPziL/5j7fv+ute9Rnt8mvjf+0THEum03VrXuuaj1mNqsaZzCRBnJYNV1a297fY9aMykcFIfLI8bLhAfhBbhwjq/ASkJbelpi3XucwT/9z/+18YCK0rHQvZmCtoXTgJJSTBIyiDIuFbaklKgzR9ZkGvLS7Kxua7B0cUTC9pDc31rXZaWNmRrKyFbW65JvzlG+RswABAm0zzjpr8wkcYAfMIWxjDW5wqRB8uEFXT72xZgQiy4rnwFXd0L0DGZDBCrTcAqDtCuNRHAA1ToP2zj/HYvjBH3aaEV2I2/OY7zci6zHE3/yqU9KSY6kvEDDaMA5t/+xMfV8/2JT3xCA8QwluehV7omsmGAab6atkTdW4zyoB5sscNR7iLmTRunHeghGt437C65gZaGWScuVWf/0nbeL/3oe28ZWPbk8SP7XlslH/lTylICsHBYuua0qUxWktqTwdM0jnqzKs0uBQ60L8rK00+vSpOOKUmYxxV3qkqYzer/Tz31lP7NpBqbGfAAhIGFwTAFHKAxIDCSlXexr0vNTe/WGmpN43AFVKvSATi2cKXFFGE6mAerjACwVUcDHOuSzH1zD5r2W63uhozYZmGpcHCabdmUSMnvyRtf90rXG0tE3vf+75LV1WX5sz/7Mzl77tSwtdKQlW5gruRIYO3Nj8tTGwUqZ+mPBlb4+JsBVnjVVwOY9/N/711jgRV1YzSbaGsGJYHUlOtsTGl8QFZhWgFGusssjfynilKpV+TS1YuyvbOh4Ysp6uI2Kfly+lH4qTafEuLG3AVMGuCyzFD2t7UD2d90EPZXz70ucpTRYwALIo/tiEhASDBWixLW1nYboXFuGA1RCVuxP8o0ACWNGf3GfFkA3yxEU9KNsdgOw7K/KfGWWcGnZi34gRSkJT/8g98vf/In/0n7PNCHFOtYxfik88EJqYwjGItkvl7XrT80mpH21oscqcp8ExjLWYQjGOtnf/AdhwCWaDe+gL5X4vqyJyQn4mVccamX0p7sLEmytrEmnu/J1PSELrJHYeX15XU5ffpu7ZTMwIR9PZbaAvuYSDFLCwC5EvdhGdOwENX8bXxaXBKWsbCNRfVxGcCApPuasm2VyoCBv3mbGLaVvmAqtgF2AOMW1nQhK7tfC2TzEFgxRdi/xnb2N2DNZAL5yR//kPzhH/5reec73ynbO5tq4PDq9V3viChg6TrRfUAVDaww+xj4whJHG5HcoGMNl/cbt9CpGmkuN+4GUfjTf+dtY4EVpWOxShc5UQDI84jjZSWZIMo+ZC4vpesMknBfa9aU3cSjjQ6LPbal3e3JVGleclnnFbdyeCaCpxo2ASD29JtXnPuxLngAxALGpsAzcTzpWHgcb0zGgFpuu+lwgIxrme/J+pYCAI4jKG0gZR+YhGvDXviZFADDUJI1FzHve9jFwr0bAA1YiMK7TszJ9/7NvyGf/exn5JFHHpGl5WsyOelK7SvV7WFEIZqxgqGDMkqqjCqG2dsXt4kD1mhGG7OCboD64XS8MLDUEv6p73v0loFliAdYfUSel9buMF4yKwkvI4lkVsTLahvCre0dmZ5zFScbWxvywpXnpdmpydGjC6qv0PQDq44fy6QzAeZ15n9r2xiOvTHBYUek1RNq+8RhIh/6FL4hdCAAZgsp2TX434BmuhyWI60dAQqAQ99CQceIYIIAmVmGsB/HuYyM/U942N1h/UwtywHQWSwSYL3mZXfJq1/xoKysLKkRQiEDFTqcc3OLxcjxEe0By/rSWz47KshB5T3MUpZuNBp4e8CKEqXjdCwyXUeKwo9875vGAitSx5Kk5CbnZACYtA4QxqJtdla7GiMa09m8rK6vuUzQQVuKdFApUpzqsjhJlSWgCuJNX3ILQvq7GQrWqpFJMgckkwOzATBzWsJCBiwmHlHFErq0r+acAANguV5UbrleroO4/PSnP60ZFpSqw1ZYibQK4poAE0BQ1MA9m4LPtdHPrOeDebldbaDL5sSNYoUUYeWd6wKsN7/ifjlxbE6tM13htd/RUi2OXd9YuUHHGgWsOFFoRsWLZRX2dJmaETrW3/9bbxgLrBvMzOGGvpcUP1sSz6eTyqQkE1kpV1pSqbZlojglR46ekK2dshRLJbWSnr/0jCb0X7jrvCSSA7l66bIcPeL0HCYAFsGURxyhFwEEWAcdh7c5MQEFEwdIANquc2+4rAjxQViD7wAY+1tyH4o5gONJBCiwJoCy5d/MIw948HfR0c/6L2hnm2RS448czz0TJIbddEHzTEb1Nr7n/rg+52UbD41Zu6zEygsReHImL4mg4zIGiAx0aCnudKvJUsFlfAitktzSInslom5CGXOAZWlA4c8wk9p2TTBEJ1aWxUG8t97gwVgwl7wZxhqpY/3E33z9TYV0RoELYHW8nHh+TiYLU5LNlaTe6MrGZlV7ihYmprQDH3lBPPmb22uSyqbkHecRMQAAF3RJREFUyMKsLu3B037fPfdp7wHzCZl+BZCYuNe//vW7K04YS5kHHfawHC/LxeJ4AMr1OB42Mscrn9bElnOFlXOzEtHNzCjg3ADLxJ6FcsKLhgNgs1Q5FrHJPdhSv4CXLnzcKy+Yk2283vltj8pDd53WwlEsQe4Bh7gr40KTcC0A4oDlJ2ljvj8120D0nwtYI3WswwKr1vWkn8jI5MSUlErz0u0lZHl1S7a3atJnxQTq26hJS5KintClaAfS1UUdeapL+Unptl1TDQbCPOuWaoOCzOCGLUYzJowBzDIzXQtAIB5hCxgJgCFabBUvjjNmAsBWQezWN5x06xsuLw/bQZYVGLgqzMdmijjnJKee7bBUOJBs+hi/A9blt8JaMBGf/H/m5FH5oQ98l9TKpPFsuT4SOReDVB9aozLseRrNWAasKMYKsxB/G2M5PWxvhdQ4d0WUxMKBGSkK/+vvfu1trwkNY7U8X1o9TxtcTE0vUBuriyptbdak1elqkw8co/nihMwdIeAZyMbOmoZ2WBatulWVbDq3m/9ksTSr62MSzA8UpmZLkwEEFjtD3JlD0JypaiAsLSlrEcxFX+OcAAsgMMEAD4Bp+dOwmILjmVzEGMchohl8jtUWQPSnH/rEbJUvW3pOG8gNwQMLoaNxLhO3/A49R6cuP/J971ddi0lW/xxN0fyE3ku1tjME862JQktc5P7s7zDw9r53ojDO8x7OmL0BYAG+wv3KuxlZ3o+9/zVjE/2idSxPksUpKTdaGs7JT0yrVYiOVW/01CpcWd3QnpiTpZLMLkzLIOjI2saKVKtl8RMp8dqe5HVxy/2+LAt7WKiH721ALNYGuCwTwHLWLfjLJDF5gM6trCqqdHMMQGLAEEvGVgABlkIkcw5roU0g2JWluZUl+N6qktEFAYAtmsm5zfVg2QwYAWznfJYnZkUiO5srcsfijNx14bTWDnLdZquuolB9dW23ysat6lhhYJmzGJDdDmOZb3AkBobAGqlj/ej7Xn1TqcmjTtxNeJJhXeYGPin6XdKNztPl4IgV5gslWVnbkPwETWcL4qUCaXddU9gmDcs6ffGaItk0rafpbe58PZZXxI8ytnKpwK7fug0cg4++AlPwvfmzXBN+l8CHYWA+KqvSMZHJPogyW7EeRgGEnNfOgQJv7MQYwJDmcLUl5zR/Peki/Bxr9809WZkWuhcABZSWnoMIrK5ckle9/AG5++47XXipDkO71uSdrmsmdxgdy4A1WlSOZyxzXUQBK1IUfvhvvCoWWDZgo4HFojVZafRo+yzS64psbNZkbb2sccDS1LR0B4EcW1zUQoG1zXWpNcoajA6kJ+XtqvQrA0mzamrOicPwKlkMKpNmDkeAxfdMHCLMsgmsUZo1WbMe69w7jGHN0yzPne2mdAMUW3wc9uKc1raIfUznAjCch2uzDxPGdazMzKwn7ldTYoZpPOxPSZs19njuuef02uiOWIMvPP4leehl9+rq9dwfCY2U0vNb0bHc+N+aKAwr75ZYeSuiMOwHi5t/dCxXsT3C8/7hv/4aXfVCG5yqxWs4Gy47hmd96LpX7V9jQ26pXFJbm4meNuVIeJRP4etZlctXl7Tl49QUCfqz2pKaZJ5nnn9aReDcwpwWR6wtr8qRqUXtg8REWElWGDhMunnemUzt+sdybEOrUZuFDPPZEWdMMEziUm7S2q3FltCFjbQl47DAAzZDB2PwOBa3AS9jMCaaCQZwvLAOLfaIWLO0aws5MSF7iwO4WCUPxRe/+EVdq/D+++9XoAMs2gzNTBbk0//h38r50ye0AqZeqSqoqRfkOptbq1LIE9KxdXFYW5BVKNwCUcxHQt0NriwuLOpMGTc/VhSwcP+YMXRjyMfpaHEv1bHIxQtlN6jn/Ufe+waSTSXQhvM9keRwnV/fyWSS6AjR9LpJ6XQ9yWZZMX5Kmo2erG6tyUZ1XXoBDkGXT06vAcBXKk3rE33PPffoE84yJvojE8HuujdsnyzRJsh5rsOxPvubSbABC4tA+yE0B5mYmNQJZRIRU3xaEQXHmtPU0l/4NHHL/SGu+B/XBNclmwEAoguxLwDlb40ebGzsxgoBJeDkGgZYwGeLkWuz/5UV/d/Sprlv2Ir9lpeuydXnntVccnpWLNKqKJsX0n4122Miq2s20xWm00X3aks644mfIFWblgNdyWRnNE0pyontmuK6sT2oY6nanXI+rShgHpyTg9dhUdFRL+9H3vto4BbzoT6wL5J0qb8EjEnCb7YxSSc1RNPuUNtXlFxmWtM1tivb0uiWpVzfUt2EQWaAmQCeeiYDtjILy27eMgCogwNwuCPC4GFQDUSmH4W37T0d4urvcgWdfPPM82kKswvi2iJGrqjV3Bfmu0K3YsAOethhPdwMsBnntBQb7sk8+5aEaKEoVcCHrYuYVL7neP6Gcbk252RsqEqenMhrQ7N0Mu0iAr5bc6eYIw9+Qra31qXfq0mvV5Ng0JCU35N0JpB0irbavs5JHLBMdRilY5HwRzapjf8oxhsHLAyakcD60He+WYHl2mj1xPOp4wv0ggBra7supal5yWanpdWB3tOSSZU0PogCni/6cm3lslI84LIJQvxYaIbBtKi+3ajmvLMcHO2qhzWDVm9nCjs/1DzrUcAq5MkQdZNmgLWkPPOYc56o1GX24TgmgIkFBGSVcs8ADRBwDwDEHKf8BtgNNwTHkTPGvvxeRC/Xwj1h3fM4Lmxg8NChwF+7clXmZ6d1ZQl+A9XMqSRdk1uSGJDyw+LrZOW2pN+j9J61reuS9HuSL6Qlm8nrnMQBy6ztOGCh6dwKY4V1MB62kcD60b/2lkDXVoG1Eq4ChhTmBKkbyaRUay2Znz8uxRKlRyLtJumoOQ0lsMJBMt2VS9deUA91OCzDwKPLIJrMsWhFC9yY+mnqNV3eZBRjmWPPRE0UsDyhG4tb9IiX/VD+57qINfPOm1hUUA/bZ3OPgNLlsGfU18QxsAaMBduaNWqMBdOZ7wqQsMaziTf+BmS0d6TNIyA0kHIdgIuKwDmvXrkkvXZLctmsnDxxWi5cuFOK+aIu57u+uqLtGY8tzkvKZ83sjnQ7VWm1dpTBXE8Uyt7I14oWhaN0LAORrgiSAFV7ojAMwHFsxXhHhXy8H/3rj1IZqEUKKIa63oqKQRjL1+DykYWTMlGclWYrkHqDZmMZCQZuscYr15+TpbXravZjlUH5JgYtKMxA87eJRG6YfTe2NmVqZkZtA3PEmZizp4D9onUstDOaVTgXBC/zH5kLINw4zXQtQGXAAiDWQojr8HDoMiqJhIIN/QuLDjbi/vmOa1n1DOf/yle+onFFzkUcEN2LADbrEprDEAbjOI6HrRiPK1cuSbNRk3wmK2fOnJOXvuR+mZ9fkFatJVubm9Jq0D5oS7LZhOSzuF5Y0JJeEjho3WIA+QJ96KNTk80qjNKxtI/sLTJWWM+KBtZ3vcXpWFpnRtWNk7sJBVZSpkpzMjWzoHoWC383WyxF5kuz0ZFKrSwvXH1GypUtV77VbuuTyAAzEVboaY5JFF/LuAQwaxvrGqB2wHatssNvi83FAWuqNKM5QWaBWSiI/zkX92GAgqnsbd8bs9lDAHj4HZp9oM38W8pcgMIV1VZVv4KRCNV87nOfUwXd1jqE7VDaARrFpvZgmOcedQEgA66lpWta3tbW5MIJuXD2TmWuXCavVTV0ePn6174sfiKQbMaT0mRaipOEfGBm2gq19MGPA5aFyEaJOhgLBegwjBXlmfc+/D6Ud+fp1XY3WAh+Sts8w1hzR45JNlfUUI2GZzqiC4KzZuDy2op0ujVpdxq7ayXDGgDLtYueUaBZyTg+HBOXTPbm9pZebxRj2Q1b3lSUKFw4cmzY8c+Bxiw/+xs9yALUVvbFPVrMDEWbQed6LuDr1vcxx6LlqwM47p2JAlCwMMexIKXlWgEinmYeJNjOAuHsxxhwDUCI+EUc0puh2aoN3RmeHD96Sk4dPyXFAg7ftORYV6ddl0p5U1rNsuRzvhxZmJbSJKu6tpTtYOuD7oIwo1iiXxSweoEjk1E62ChLPXzuODeE9+H3vVlFoZbisyd1ZwmWc0upm2H+yKKW+ACmRtO1dGx1KH1akStXLks6A9DcKu+IBSYHQPHEMgG20igKMCKDJ9Z0DBr/swqF+tGGJm/40wYsjrGmp2Y1EGoJdPqADJP9uBeAabnylqFqbASALMOB7zgHD4Lla1nCHvdhhZ8wE29+K+EkfitAwtXCbwNw2m99WBACyxmwYEmAhd+NfTY316XZIZmxIulUTk6fPKe9QpPiVt/od3vy0AMvlatXLsq1q5fEC9oyf2RKm9F2e3WpVysyPUVB6ejVzRjLUbHCsI6lfRwOASzTww6CzPvI978jWN/alkJ+QtJZVo2vSmlqTuYXTki9gYLd1pXOKYzAi07X3WrNtb2mmLTR2Jb19bXdpxmGAFCWBUqSHRMAM6B7wGZMphYqJBNy8dIl/WHmEjCRZtkMFsIxxjKLxDGap52UEYUAwzXWcP0PzJNvWaiczxRZ802ZcRF27llI5qAeYaLT9Dj7nwcIscZ4ABquiyi0fC+LM4ZzuqxyiGDz/NFpefzJJySfK8qbXv8mmZtZkOefvagLf144d05qlaq87KX36oJKf/n5/1eWr1+S02eOyYmTx6TXbUrQ7+jScebtNwscpuW+GI+DwLMxRBSmstQnRAPTABMej7BVGMVa3k/94LuCrZ1tSflZyWTz0m4F4qdzmg26vlmWyZJzwPWF0mvaD7WlUq8NgVWTanlTmo36bl2gPdGWO26ihCeUN+yBok/Al75NjiX3mj/ANuF44UFRaGLLgUG0PzzAMlZiIBkECw+FPeW7FcRD8HGPvJiMcJGpPemA2fxynMcC26aDsQ32sQm0Fe95sAAvij/jYaxhmaWWRUEFaKGUlUuXLkomXZCHH36FHF84KVsbO4LjkR5WNOsoTRZkciIrzUZFrl29KOXKhhxbmJVz509Lt1UR0bYDzvNuDwnXtHSiKOcpjyZ99m38zRIPi8A4NrTvRoHL+wcffGfQbGFhYAli7VGcEMh2uSGbW1V5yX0PSJ+YENF2yq+arClYlXqj6tZEXr6m92UOSQaatwVxuUlMcSxFFF9AhXhAX8HdkMUznNzz/pooMyV+nI61RvYEHZSH6cBMGte08BATayxm4SBjDIBhtYkWdzR3hVmViDjr5wmwuG9zQVj9odUxcl5dGXV+XhkURZ7fzt+m39nEq5jtd8TLEJ7a1JqBs6fPypnTd0o+U5BOq6t5aumkLxtrqzJ/ZEbuvfuCrK9dl7/43J9KIZ+RN77+leIFdRloXwu36IH5Ao1ZTXkfBS7neWepvr3Ix0EdKs7lEAusD3/fm4JcoSj1GiKuI8EgLds7DSlXmpLJTsq9972MVkraBwBPOZmO1XpZO5F02y1ZunJZMmmXtclEQfkMLiKPTwMVk4H5jaVkOhNr4ABWgGXbTCQawMxcDqeC7IEvkO0tUndd2MbcCAyG3QugMIYxhrK6QFPSOdb0KQv1mNsCkc3v4FjOa5U45nTlf/PTcYz9dkQ/ohFVgAfK4pgm2tWg6NRl4Pe0DUG/F8j05IxcOH+vnDp+Rnrtnq7ulUmn5fLFF7Rs7lWvfES8QVe++IXPSb1RlrvvPiOLcwTGXatK3pZ/z70Y0KNYR6PCyNGhxAjrt2HdKYrxYpX3H3j/K4LjJ07L2uq2XLm8IoFkZafckkEvJfe+5AHJ5IpCnBEA4GlvNCvSaNU0b4hWQ1X8OgkXRDalGBMfKwhwsZ4Lk4VH+rHHHtMBtvUAsQq7JMwNPe9GxQYc+zSatx9r4hJRSAtFtwqDc45aiMHcCMYq3Jsr/nSrqnJPljtvTzngNevQ/FyWdsx2YzjuB0BZMYaJII3vDdnaEgjRwQxY3LfpQtxru9sQyjARiawAlvazcvrUHXLhzB0S9BOyvrKufaxY5YteWOfOnpKFo7NSr27L0tJ1qZbX5MH7Tkux6KqwLZSFfsu1MDAwlKKAZe6GW9GxbI7i2Eq/+/7vfkVw7tx5eerJi/KNJy5qo45apauxwde/4a1y7fqadoAj/NJs111KcaehztGg35ZkP5ABXf2GrX2sLSMiyBRXxAdhEjPXYTd+9NLKskzPzu5aJTcCx/UVtSfJ2MyARc/3bofiTxfdD1tx6BdMooVs+J774dN6TPGddZCxILHpXADUzmcri3Hf/C5EH6BiuwHXgGqefDMmTK/kfBr1HwaENVM1aEummNI+Vf0uw5CSuZljsnBkUXJ+XvuM0m+lxqIIlW3JZZJy5tQpmZufkpWlK/LUk1+Re+9alLOnFtRY4p6YBwOTLfgUFmdhYAAs5jYKWGHw3CpreR/+u28KFhdPymf/5PPy1JMvyH33vUJaDZGtraa88tVvkMtXlvXirG7Q0n5VNen03SpQ9GfyB3Q0cYUC/DjcCwy+BX8BBnoVii0TYykrmuLbbEgml9v9YWGLKyySwsqkOnJ3kwHpDQVbOs+7uRnYx4wG/mbAOQfizPK7jMH4DAemzX9mYIYFAJ8FlvmNZphYtiqMZGwB6MIg5zymr3F/BljVtfyB5EopaXSaunxexmeVioL4yYyUcjPaejsRJGRne1O2CZQHXTkyPytT00Vpt+kTuiTpZE0efuAetUSdpd5S1mQ8uC8T/6OsOoCFinMrjBV+yKNcDQrIn/rx7wxKk7Py7/7Pfy8vXFyWt7z52zU95qlnLst99z4oFfqOs2xbr61eYhRO66xHq992vSa9LuVLSVVacSkgCi3KzwQ//fTTCiy2W2sgBr80PSXXlpZ202UMNJbWwjnClsqNwKNRq3NmhsVYWIm1xiBsM2Dxt5ZX6fJt87tr6TAxXNtyw2AxtnEf9uY6FgJSpb3nenuxn1UIMeDsz3VMz2FfgGWTr5ZnOim5UkZ26mXxBuSjFWTQ8XSh9enSrJw5eUZ/1/L1JS29T/tJbTtEejctuednCnLt4tflDa99uaocnJsHliV8YWurB1BQkVgwZEv7RMeiTeRhrMJR3fwUWL/+az8d/P7v/77SMEuL9LqBzB89JrVqS5eWxb2A34on1gaVA7XEvbKt/hVaZfPDCNk88MADCiImDUXdyp5GPTFsQ4Hn0/Spg36sXZ9LhKaoZf4hq9LAd9C6tJgd5zOl3YpV+W22fC/A4neyj73D51LviK2/yAIAHbe/sZLFLE1s27iZDmiFrDoefkIyhYyGVTxbfaNHWGsY8xx4GpN0TOyqv3t6f55r2JtPSWtrTaaKeXnve9+rbLW8TEjphKY4u4fEWdzWGNg9qMNkTq2eoplLnBoe5+NKSCYd0Sryn/3Kzyiw8LQ7ncIlvCmwikVptbpOFDRcDV+O8E7gkvXocpzNobQntY8Tnmc6/AIwjjVgxcnncNqF6U7heF8c3WooSBcL3J8oGHZ4hlnOnJ9maPB7YFHEk4k7y2+36mXAFQesAZU1rIw1NF7CBgjXDgOL64SBRZNZVAEX+dhbEN1cLWxDrWAMTIxyDnMAFzK+9CsVmcxn5D3veY8aU8zByVOL+nv43zGVA5KXICXqmwssIgZaLXPg5f3yL/xk8Ad/8AcKLKdr9FU8sNi3U0zdIpM4IvmB6bRLDXYe7YpMzyDPE5rQh9OTqD6pLgCLQgUGIg5Y4SfczH5TmsPe7lHPlAHLdITwPqYrMeEGWBNPYbMa8WjXte/DrATA4oAV0DkmBliMnf1Gc+Ka5QrT4seLAxYi20S96ZB8qgVO71Ca1PmeAgsmw0DCK+/aDvQ1h/5bAqyf+5kPBZ/85Ccl5buGGgBLY2E79WHDVreKBNaX6gVJtyg3Tx7VNovHSecVLVvHIUiqr7VohLHMqjML46DOZN7wMLOEGcsCw1HAIp/LJiaseBuYDRimI4XZwJT8XdGmT/NeeIlrWoDZjjsoCpFbccBi7Ez/Qy2w7AmuDWONAxb3DbjQ97hPxsZSVdJ47mGifkfe/e53a3twJMni8YXhihquZP/FBBaGxijG+v8AO4h5o2flURoAAAAASUVORK5CYII=' alt=''>");
            client.println("<h3><font color='9acd32'>WETBIRD V103F23-19 BSW</h3><font color='994f00'>");
            client.println("<h2>Location - Test</h2><font color='black'>");
            // client.println("<h3>Temperature in Celsius: ");
            // client.println(celsiusTemp);
            // client.println("*C</h3>");
            client.println("<h3>Temperature: ");
            client.println(fahrenheitTemp);
            client.println("*F<br>Humidity: ");
            client.println(humidityTemp);
            client.println("%</h3><h3>");
            client.println("Readings are up to 2 seconds old.</h3></div>");
            client.println("</body></html>");     
            break;
        }
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        }
        else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }  
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
    digitalWrite(0, HIGH);
  }
}   
