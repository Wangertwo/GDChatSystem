const nodemailer = require('nodemailer')
const config_module = require("./config")

/*创建发送邮件的代理 */
let transport = nodemailer.createTransport({
    host: 'smtp.163.com',
    port: 465,
    secure: true,
    auth: {
        user: config_module.email_user, // 发送方邮箱地址
        pass: config_module.email_pass  // 邮箱授权码或密码
    }
});

/*
发送邮件的接口函数
@param {*} mailoptions_  发送邮件的参数
@return
*/
function SendMail(mailoptions_) {
    return new Promise(function(resolve, reject) { // 返回Promise，外部用await接收
        transport.sendMail(mailoptions_, function(error, info) {
            if(error) {
                console.log(error);
                reject(error);
            }else {
                console.log('邮件已经发送：' + info.response);
                resolve(info.response);
            }
        });
    })
}

module.exports.SendMail = SendMail