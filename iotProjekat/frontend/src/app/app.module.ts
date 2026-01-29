import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { HttpClientModule } from '@angular/common/http';
import { FormsModule } from '@angular/forms';

import { AppComponent } from './app.component';
import { IotMonitorComponent } from './iot-monitor/iot-monitor.component';
import { SensorService } from './services/sensor.service';

@NgModule({
  declarations: [
    AppComponent,
    IotMonitorComponent
  ],
  imports: [
    BrowserModule,
    HttpClientModule,
    FormsModule
  ],
  providers: [SensorService],
  bootstrap: [AppComponent]
})
export class AppModule { }

