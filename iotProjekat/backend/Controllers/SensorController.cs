using Microsoft.AspNetCore.Mvc;
using backend.Models;
using backend.Services;

namespace backend.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class SensorController : ControllerBase
    {
        private readonly FirebaseService _firebaseService;

        public SensorController(FirebaseService firebaseService)
        {
            _firebaseService = firebaseService;
        }

        [HttpGet("data")]
        public async Task<IActionResult> GetSensorData()
        {
            var data = await _firebaseService.GetSensorDataAsync();

            if (data == null)
            {
                return StatusCode(500, new { message = "Failed to retrieve sensor data from Firebase" });
            }

            return Ok(data);
        }

        [HttpPut("gaslimit")]
        public async Task<IActionResult> UpdateGasLimit([FromBody] GasLimitUpdate update)
        {
            if (update.GasLimit < 0)
            {
                return BadRequest(new { message = "Gas limit must be a positive number" });
            }

            var success = await _firebaseService.UpdateGasLimitAsync(update.GasLimit);

            if (!success)
            {
                return StatusCode(500, new { message = "Failed to update gas limit in Firebase" });
            }

            return Ok(new { message = "Gas limit updated successfully", gasLimit = update.GasLimit });
        }
    }
}
