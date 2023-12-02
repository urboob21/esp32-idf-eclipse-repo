# Edit following two lines to set component requirements (see docs)
<<<<<<< HEAD
idf_component_register(SRCS main.c rgb_led.c wifi_app.c a_http_server.c
=======
idf_component_register(SRCS main.c rgb_led.c wifi_app.c http_server.c dht22.c
>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
						INCLUDE_DIRS "."
						EMBED_FILES webpage/app.css webpage/app.js webpage/favicon.ico webpage/index.html webpage/jquery-3.3.1.min.js)					
