#!/usr/bin/python

from flask import Flask, send_from_directory, redirect, request
import json
import netifaces


def create_app():
    app = Flask(__name__)
    filename = './airball-settings.json'

    @app.route('/')
    def route_root():
        return redirect('/app/index.html', code=302)
    
    @app.route('/app/')
    def route_app_slash():
        return redirect('/app/index.html', code=302)    
    
    @app.route('/app/<path:path>')
    def route_app(path):
        return send_from_directory('./app', path)

    @app.route('/settings', methods = ['GET', 'POST'])
    def route_settings():
        if request.method == 'GET':
            with open(filename, 'r') as f:    
                return json.load(f)
        if request.method == 'POST':
            with open(filename, 'w') as f:
                json.dump(request.json, f)
                return 'ok'
        return route_root()

    return app

if __name__ == '__main__':
    create_app().run(debug=False,
                     host=netifaces.ifaddresses('wlan0')[2][0]['addr'],
                     port=80)
