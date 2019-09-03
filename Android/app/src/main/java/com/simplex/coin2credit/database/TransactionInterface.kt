package com.simplex.coin2credit.database

interface TransactionInterface {
    fun relayMessage(data: String)
    fun messageWritten(success: Int)
    fun databaseRead(transactions: List<Transaction>)
}