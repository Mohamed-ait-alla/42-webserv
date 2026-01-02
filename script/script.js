import axios from "axios";
import chalk from "chalk";
import fs from "fs";

const data =
  '<!DOCTYPE html><html lang="en"><head><meta charset="UTF-8" /><meta name="viewport" content="width=device-width, initial-scale=1.0" /><script src="https://cdn.jsdelivr.net/npm/@tailwindcss/browser@4"></script><!-- <link rel="stylesheet" href="style.css"> --><title>Home</title></head><body class="w-full h-[100vh] flex justify-center items-center"><h1 class="font-bold italic bg-black text-white p-3 rounded cursor-pointer transition-all border hover:border-black hover:bg-white hover:text-black">HELLO FROM WEBSERV</h1></body></html>';

fs.writeFile("index.html", data, (err) => {
  if (err) {
    console.log("file not created !");
  }
  console.log("file is created !");
});

const domainame = "http://localhost";
const port = 8080;
const numRequests = 10;

// * GET
// const GET_METHOD = async () => {
//   const promises = [];

//   for (let i = 0; i < numRequests; i++) {
//     promises.push(
//       axios
//         .get(`${domainame}:${port}`)
//         .then((res) => res.status)
//         .catch((err) => err.message)
//     );
//   }

//   const results = await Promise.all(promises);

//   const numRequestSuccess = results.filter((value) => value === 200).length;
//   const numRequestFailed = results.filter(
//     (value) => value === "socket hang up"
//   ).length;

//   if (numRequestSuccess === 0 && numRequestFailed === 0) {
//     console.log(chalk.white.bgRed.bold("╭──────────────────────────────╮"));
//     console.log(chalk.white.bgRed.bold("│     YOUR SERVER IS DOWN !    │"));
//     console.log(chalk.white.bgRed.bold("╰──────────────────────────────╯"));
//     return;
//   }

//   console.log(chalk.white.bgGreen.bold("╭──────────────────────────────╮"));
//   console.log(chalk.white.bgGreen.bold("│          GET REQUEST         │"));
//   console.log(chalk.white.bgGreen.bold("╰──────────────────────────────╯"));

//   console.log(
//     chalk.white.bgBlue.bold(`  Total Requests : ${numRequests} 🚀  `)
//   );
//   console.log(
//     chalk.white.bgGreen.bold(`  Success        : ${numRequestSuccess} ✅  `)
//   );
//   console.log(
//     chalk.white.bgRed.bold(`  Hangs          : ${numRequestFailed} ❌  `)
//   );
//   console.log(
//     chalk.white.bgMagenta.bold(
//       `  Success rate   : ${((numRequestSuccess / numRequests) * 100).toFixed(
//         2
//       )}% 📊  \n`
//     )
//   );
// };

// * POST
// const POST_METHOD = async () => {
//   const promises = [];

//   for (let i = 0; i < numRequests; i++) {
//     promises.push(
//       axios
//         .get(`${domainame}:${port}`)
//         .then((res) => res.status)
//         .catch((err) => err.message)
//     );
//   }

//   const results = await Promise.all(promises);

//   const numRequestSuccess = results.filter((value) => value === 200).length;
//   const numRequestFailed = results.filter(
//     (value) => value === "socket hang up"
//   ).length;

//   console.log(chalk.white.bgYellow.bold("╭──────────────────────────────╮"));
//   console.log(chalk.white.bgYellow.bold("│         POST REQUEST         │"));
//   console.log(chalk.white.bgYellow.bold("╰──────────────────────────────╯"));

//   console.log(
//     chalk.white.bgBlue.bold(`  Total Requests : ${numRequests} 🚀  `)
//   );
//   console.log(
//     chalk.white.bgGreen.bold(`  Success        : ${numRequestSuccess} ✅  `)
//   );
//   console.log(
//     chalk.white.bgRed.bold(`  Hangs          : ${numRequestFailed} ❌  `)
//   );
//   console.log(
//     chalk.white.bgMagenta.bold(
//       `  Success rate   : ${((numRequestSuccess / numRequests) * 100).toFixed(
//         2
//       )}% 📊  `
//     )
//   );
// };

// GET_METHOD();
// POST_METHOD();
