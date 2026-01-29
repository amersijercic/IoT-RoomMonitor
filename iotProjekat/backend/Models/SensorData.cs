namespace backend.Models
{
    public class SensorData
    {
        public double Temperature { get; set; }
        public double Humidity { get; set; }
        public int Gas { get; set; }
        public int GasLimit { get; set; }
    }

    public class GasLimitUpdate
    {
        public int GasLimit { get; set; }
    }
}
